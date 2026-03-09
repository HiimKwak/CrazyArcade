#pragma once

class IWorldQueryService;

class IWorldQueryProvider
{
public:
	virtual ~IWorldQueryProvider() = default;
	virtual IWorldQueryService* GetWorldQueryService() = 0;
};