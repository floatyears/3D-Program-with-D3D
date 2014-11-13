#include <Windows.h>
#include <xnamath.h>
#include <iostream>
using namespace std;

ostream& operator<<(ostream& os,FXMVECTOR v)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest,v);
	os << "(" << dest.x << ","<< dest.y << ","<< dest.z << ")";
	return os;
}

int main1()
{
	cout.setf(ios_base::boolalpha);
	if(!XMVerifyCPUSupport())
	{
		cout << "xna math not suppported" <<endl;
		return 0;
	}

	XMVECTOR n = XMVectorSet(1.0f,0.0f,0.0f,0.0f);
	XMVECTOR p = XMVectorZero();
	XMVECTOR q = XMVectorSplatOne();
	XMVECTOR u = XMVectorSet(1.0f,2.0f,3.0f,0.0f);
	XMVECTOR v = XMVectorSet(-2.0f,1.0f,3.0f,0.0f);
	//XMVECTOR v = XMVectorReplicate(-2.0f);
	XMVECTOR w = XMVectorSplatZ(u);

	XMVECTOR a = u + v;
	XMVECTOR b = u - v;
	XMVECTOR c = 10.0f * u;
	XMVECTOR L = XMVector3Length(u);
	XMVECTOR d = XMVector3Normalize(u);
	XMVECTOR s = XMVector3Dot(u,v);
	XMVECTOR e = XMVector3Cross(u,v);

	XMVECTOR projW;
	XMVECTOR perpW;
	XMVector3ComponentsFromNormal(&projW,&perpW,w,n);
	bool equal = XMVector3Equal(projW +perpW,w)!= 0;
	bool notEqual = XMVector3NotEqual(projW+perpW,w)!=0;
	XMVECTOR angleVec = XMVector3AngleBetweenVectors(projW,perpW);
	float angleRadians = XMVectorGetX(angleVec);
	float angleDegress = XMConvertToDegrees(angleRadians);
	cout << "p = " << p << endl;
	cout << "q = " << q << endl;
	cout << "u = " << u << endl;
	cout << "w = " << w << endl;
	cout << "v = " << v << endl;

	cout << "a = u + v =" << a << endl;
	cout << "b = u - v =" << b << endl;
	cout << "c = 10 * u =" << c << endl;
	cout << "d = u / ||u|| =" << d << endl;
	cout << "e = u x v =" << e << endl;
	cout << "L = ||u|| =" << L << endl;
	cout << "s = u.v " << s << endl;
	cout << "projW = " << projW << endl;
	cout << "perpW = " << perpW << endl;
	cout << "projW + perpW ==  w =" << equal << endl;
	cout << "projW + perpW !=  w =" << notEqual << endl;
	cout << "angle = " << angleDegress << endl;

	return 0;
}