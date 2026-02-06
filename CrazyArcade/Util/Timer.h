#pragma once

class Timer
{
public:
	Timer(float targetTime = 1.0f);

	// 타이머 업데이트
	void Tick(float deltaTime);

	// 경과시간 리셋
	void Reset();

	// 설정한 시간만큼 흘렀는지 확인
	bool IsTimeout() const;

	// 목표 시간 설정
	void SetTargetTime(float newTargetTime);

	// 남은 시간 반환
	float GetRemainingTime() const;

private:
	// 경과 시간 확인
	float elapsedTime = 0.0f;
	// 타이머 목표 시간
	float targetTime = 0.0f;
};
