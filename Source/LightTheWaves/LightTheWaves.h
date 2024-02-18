// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define ECC_3DWidget ECC_GameTraceChannel1

#define TEST_BIT(Bitmask, Bit) (((Bitmask) & (1 << static_cast<uint8>(Bit))) > 0)

DECLARE_LOG_CATEGORY_EXTERN(CBLog, Log, All)
