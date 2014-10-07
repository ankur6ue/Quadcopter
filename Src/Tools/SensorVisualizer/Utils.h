typedef struct AxAngle
{
	AxAngle(){};
	AxAngle(float _angle, float _x, float _y, float _z)
	{
		angle = _angle; x = _x; y = _y; z = _z;
	}
	float angle;
	float x;
	float y;
	float z;
};