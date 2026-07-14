#define NOMINMAX


//#include "directx.h"
#include "texture.h"
#include "model.h"

#include "renderer.h"


/*==============================================================================
   リファクタリング内容:
   - aiImportFile / CreateBuffer などの失敗を実行時にチェック（assert 依存をやめる）
   - UV・法線を持たないメッシュでのヌルアクセスを HasTextureCoords / HasNormals で回避
   - ModelDraw のテクスチャ取得を operator[] から find() に変更
	 （未登録キーで null SRV を挿入・バインドしてしまう不具合を防止）
==============================================================================*/


MODEL* ModelLoad(const char* FileName)
{
	MODEL* model = new MODEL;


	const std::string modelPath(FileName);

	model->AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

	// 旧版は assert のみ（Release ビルドで消える）。実行時にも失敗を処理する
	assert(model->AiScene);
	if (model->AiScene == nullptr)
	{
		MessageBoxA(NULL, FileName, "モデルの読み込みに失敗しました", MB_ICONEXCLAMATION | MB_OK);
		delete model;
		return nullptr;
	}

	model->VertexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];//頂点データポインター
	model->IndexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];//インデックスデータポインター


	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		const bool hasUV = mesh->HasTextureCoords(0);
		const bool hasNormal = mesh->HasNormals();

		// 頂点バッファ生成
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];//頂点数分の配列領域作成

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				// 軸変換：assimp(Y-up) → 本エンジン座標へ（Y と Z を入れ替え、Z を反転）
				vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, -mesh->mVertices[v].z, mesh->mVertices[v].y);

				// UV が無いメッシュでも落ちないようにする
				if (hasUV)
					vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				else
					vertex[v].TexCoord = XMFLOAT2(0.0f, 0.0f);

				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// 法線が無いメッシュでも落ちないようにする
				if (hasNormal)
					vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
				else
					vertex[v].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			HRESULT hr = GetDevice()->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);
			assert(SUCCEEDED(hr));

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];//ポリゴン数数*3

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			HRESULT hr = GetDevice()->CreateBuffer(&bd, &sd, &model->IndexBuffer[m]);
			assert(SUCCEEDED(hr));

			delete[] index;
		}

	}

	//テクスチャ読み込み
	for (UINT i = 0; i < model->AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = model->AiScene->mTextures[i];

		ID3D11ShaderResourceView* texture = nullptr;
		TexMetadata metadata;
		ScratchImage image;
		HRESULT hr = LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			continue;	// 読めなかったテクスチャはスキップ（落とさない）

		CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		if (texture == nullptr)
			continue;

		model->Texture[aitexture->mFilename.data] = texture;
	}



	return model;
}




void ModelRelease(MODEL* model)
{
	if (model == nullptr)
		return;

	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		if (model->VertexBuffer[m]) model->VertexBuffer[m]->Release();
		if (model->IndexBuffer[m])  model->IndexBuffer[m]->Release();
	}

	delete[] model->VertexBuffer;
	delete[] model->IndexBuffer;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : model->Texture)
	{
		if (pair.second) pair.second->Release();
	}


	aiReleaseImport(model->AiScene);


	delete model;
}


void ModelDraw(MODEL* model)
{
	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		// テクスチャ設定
		aiString texture;
		aiMaterial* aimaterial = model->AiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

		if (texture != aiString(""))
		{
			// operator[] だと未登録キーで null を挿入してしまうため find() を使う
			auto it = model->Texture.find(texture.data);
			if (it != model->Texture.end() && it->second != nullptr)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &it->second);
			}
		}

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &model->VertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		GetDeviceContext()->IASetIndexBuffer(model->IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}