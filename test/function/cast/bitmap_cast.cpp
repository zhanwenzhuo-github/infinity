//
// Created by jinhai on 22-12-23.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/bitmap_cast.h"
#include "common/types/info/varchar_info.h"

class BitmapCastTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(BitmapCastTest, bitmap_cast0) {
    using namespace infinity;

    // Try to cast bitmap type to wrong type.
    {
        BitmapT source;
        source.Initialize(128);
        for(i64 j = 0; j < 128; ++ j) {
            if(j % 2 == 0) {
                source.SetBit(j, true);
            } else {
                source.SetBit(j, false);
            }
        }

        TinyIntT target;
        EXPECT_THROW(BitmapTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        BitmapT source;
        source.Initialize(128);
        for(i64 j = 0; j < 128; ++ j) {
            if(j % 2 == 0) {
                source.SetBit(j, true);
            } else {
                source.SetBit(j, false);
            }
        }
        VarcharT target;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type);
        col_varchar.Initialize();

        EXPECT_THROW(BitmapTryCastToVarlen::Run(source, target, &col_varchar), NotImplementException);
    }
}


TEST_F(BitmapCastTest, bitmap_cast1) {
    using namespace infinity;

    // Call BindBitmapCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal16);
        EXPECT_THROW(BindBitmapCast(target_type), TypeException);
    }

    DataType source_type(LogicalType::kBitmap);
    ColumnVector col_source(source_type);
    col_source.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value v = Value::MakeBitmap(bitmap);
        col_source.AppendValue(v);
        Value vx = col_source.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value vx = col_source.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBitmap);
        EXPECT_EQ(vx.value_.bitmap, bitmap);
    }
    // cast bitmap column vector to varchar column vector
    {
        DataType target_type(LogicalType::kVarchar);
        auto source2target_ptr = BindBitmapCast(target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type);
        col_target.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
}