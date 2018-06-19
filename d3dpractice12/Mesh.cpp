#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh()
{
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f,0.f,0.f), XMFLOAT4(0.f, 0.f, 0.f, 1.f));

}

CMesh::CMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}


CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release(); 
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer->Release(); 
	m_pd3dVertexUploadBuffer = NULL; 
	if (m_pd3dIndexUploadBuffer) 
		m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;

}



void CMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1,&m_d3dVertexBufferView);
	Render(pd3dCommandList, 1);
}

void CMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView); 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else { pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0); }
	//
	//pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	//pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology); 
	//if (m_pd3dIndexBuffer) {
	//	pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
	//	pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	//}
	//else { pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0); }
}

void CMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	//���� ���� ��� �ν��Ͻ� ���� �並 �Է�-���� �ܰ迡 �����Ѵ�.
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_d3dVertexBufferView, d3dInstancingBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, _countof(pVertexBufferViews), pVertexBufferViews);
	Render(pd3dCommandList, nInstances); 
}

CTriangleMesh::CTriangleMesh()
{
}

CTriangleMesh::CTriangleMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	//�ﰢ�� �޽��� �����Ѵ�.
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex); 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/*����(�ﰢ���� ������)�� ������ �ð���� ������� ������, ���, �Ķ������� �����Ѵ�. 
	RGBA(Red, Green, Blue, Alpha) 4���� �Ķ���͸� ����Ͽ� ������ ǥ���Ѵ�.
	�� �Ķ���ʹ� 0.0~1.0 ������ �Ǽ����� ������.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)); 
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));
	//�ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�. 
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);
	//���� ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, float fWidth, float fHeight, float fDepth)
{
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 1.0f, fy = fHeight * 1.0f, fz = fDepth * 1.0f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 
	CDiffusedVertex pVertices[8]; 
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR); 
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. 
	�ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�.
	��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/ 
	m_nIndices = 36;
	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ�� 
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//�� �޸�(Back) �簢���� ���� �ﰢ��  
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7; 
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ�� 
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��   
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5; 
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��   
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��  
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;
	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT; 
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices; 
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.f, 0.f, 0.f, 1.f));


}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex); m_nOffset = 0;
	m_nSlot = 0; m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//���� �׸��� ���� ����� �޽��� ǥ���ϱ� ���� ���� �������̴�. 
	CDiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	XMFLOAT4 RAND = RANDOM_COLOR;
	//����� �޽��� ���� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), RANDOM_COLOR);
	//����� �޽��� �Ʒ��� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), RANDOM_COLOR);
	//����� �޽��� ������ ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), RANDOM_COLOR);
	//����� �޽��� ����/������ ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), RANDOM_COLOR);
	//����� �޽��� ���� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), RANDOM_COLOR);
	//����� �޽��� ����/���� ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), RANDOM_COLOR); 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), RANDOM_COLOR);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, 
		pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.f, 0.f, 0.f, 1.f));


}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}



CWallMesh::CWallMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, float fWidth , float fHeight , float fDepth)
{
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.1f, fy = fHeight * 0.1f, fz = fDepth * 0.1f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 
	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(255, 0, 0, 0));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(255, 0, 0, 0));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(255, 0, 0, 0));
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(255, 0, 0, 0));
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(255, 0, 0, 0));
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(255, 0, 0, 0));
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(255, 0, 0, 0));
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(255, 0, 0, 0));
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_nIndices = 24;
	UINT pnIndices[24];
	//�� ����(Top) �簢���� ���� �ﰢ�� 
	pnIndices[0] = 0; pnIndices[1] = 5; pnIndices[2] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[3] = 1; pnIndices[4] = 5; pnIndices[5] = 0;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��   
	pnIndices[6] = 1; pnIndices[7] = 6; pnIndices[8] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[9] = 2; pnIndices[10] = 6; pnIndices[11] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��   
	pnIndices[12] = 2; pnIndices[13] = 7; pnIndices[14] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[15] = 3; pnIndices[16] = 7; pnIndices[17] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��  
	pnIndices[18] = 6; pnIndices[19] = 4; pnIndices[20] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[21] = 7; pnIndices[22] = 4; pnIndices[23] = 6;
	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CWallMesh::~CWallMesh()
{
}