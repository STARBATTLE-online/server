#pragma once
#include "HeadSprite.h"

//��� ���� ��������� ��� �����������. �� ������ ���� ���� ������� ������))
//�� ���� ���������� ������ ���� �����, ����� ��� �������� �������.
//�� � �������� ���� �������� ������� � ������������� � ���������.
class SuperPower : public HeadSprite
{
public:
	SuperPower() {};
	
	~SuperPower(){};

private:

};

class SuperPowerIcon : public HeadSprite
{
public:
	SuperPowerIcon();
	~SuperPowerIcon();

private:

};


//���-�� ���� ������� ����� ����� ���� �� ������� ������ (��������� ����).
//�� ������ ���� ������ ����, ��� ��� ��� ���� ����� ������. �� � ���� ������� ���������, ��� ���� ���, �� �� ������� ��������.
//��� ���-�� ���� �������� ���� � ������ �������, �� ��� ��������.
//��� � ������� ���� ���� �������� ���, ���� ������� ���������, � �� �����������.
//����� ��� �������� �� ��� ���������, ��������, ��� ������ �������� ����� ��, � ���� ��� �����������))
class Shield : public SuperPower
{
public:
	Shield(){
		width = 63;
		height = 63;
		shield_duration = 80;
		start_time = 0; // TODO: getTickCount();
	};
	~Shield(){};

private:
	unsigned int shield_duration;
	unsigned int start_time;
	std::vector<const char*> frames;
};

class Rocket : public SuperPower
{
public:
	Rocket(){};
	~Rocket(){};

private:

};