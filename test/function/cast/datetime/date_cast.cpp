//
// Created by jinhai on 22-12-23.
//


#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/date_cast.h"
#include "common/types/info/varchar_info.h"

class DateCastTest : public BaseTest {
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

TEST_F(DateCastTest, date_cast0) {
    using namespace infinity;

    // Try to cast date type to wrong type.
    {
        DateT source;
        TinyIntT target;
        EXPECT_THROW(DateTryCastToFixlen::Run(source, target), FunctionException);
        EXPECT_THROW(DateTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        DateT source;
        DateTimeT target;
        EXPECT_THROW(DateTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        DateT source;
        TimestampT target;
        EXPECT_THROW(DateTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        DateT source;
        TimestampTZT target;
        EXPECT_THROW(DateTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        DateT source;
        VarcharT target;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type);
        col_varchar.Initialize();

        EXPECT_THROW(DateTryCastToVarlen::Run(source, target, &col_varchar), NotImplementException);
    }
}


TEST_F(DateCastTest, date_cast1) {
    using namespace infinity;

    // Call BindDateCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal16);
        EXPECT_THROW(BindDateCast(target_type), TypeException);
    }

    DataType source_type(LogicalType::kDate);
    ColumnVector col_source(source_type);
    col_source.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDate(DateT(static_cast<i32>(i)));
        col_source.AppendValue(v);
        Value vx = col_source.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_source.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDate);
        EXPECT_FLOAT_EQ(vx.value_.date.value, static_cast<i32>(i));
    }

    // cast date column vector to datetime column vector
    {
        DataType target_type(LogicalType::kDateTime);
        auto source2target_ptr = BindDateCast(target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type);
        col_target.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
    // cast date column vector to timestamp column vector
    {
        DataType target_type(LogicalType::kTimestamp);
        auto source2target_ptr = BindDateCast(target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type);
        col_target.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
    // cast date column vector to timestamp timezone column vector
    {
        DataType target_type(LogicalType::kTimestampTZ);
        auto source2target_ptr = BindDateCast(target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type);
        col_target.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast date column vector to varchar column vector
    {
        DataType target_type(LogicalType::kVarchar);
        auto source2target_ptr = BindDateCast(target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type);
        col_target.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
}