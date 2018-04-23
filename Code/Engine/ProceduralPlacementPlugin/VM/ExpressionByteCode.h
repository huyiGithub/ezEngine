﻿#pragma once

#include <ProceduralPlacementPlugin/Basics.h>
#include <Foundation/Containers/DynamicArray.h>

class EZ_PROCEDURALPLACEMENTPLUGIN_DLL ezExpressionByteCode
{
public:
  struct OpCode
  {
    enum Enum
    {
      // Unary
      FirstUnary,

      Abs_R,
      Sqrt_R,

      Mov_R,
      Mov_C,
      Mov_I,
      Mov_O,

      LastUnary,

      // Binary
      FirstBinary,

      Add_RR,
      Add_CR,

      Sub_RR,
      Sub_CR,

      Mul_RR,
      Mul_CR,

      Div_RR,
      Div_CR,

      Min_RR,
      Min_CR,

      Max_RR,
      Max_CR,

      LastBinary,

      Call,

      Count
    };
  };

  typedef ezUInt32 StorageType;

  ezExpressionByteCode();
  ~ezExpressionByteCode();

  const StorageType* GetByteCode() const;
  const StorageType* GetByteCodeEnd() const;

  ezUInt32 GetNumInstructions() const;
  ezUInt32 GetNumTempRegisters() const;

  static OpCode::Enum GetOpCode(const StorageType*& pByteCode);
  static ezUInt32 GetRegisterIndex(const StorageType*& pByteCode, ezUInt32 uiNumRegisters);
  static ezSimdVec4f GetConstant(const StorageType*& pByteCode);
  static ezUInt32 GetFunctionNameHash(const StorageType*& pByteCode);
  static ezUInt32 GetFunctionArgCount(const StorageType*& pByteCode);

  void Disassemble(ezStringBuilder& out_sDisassembly) const;

private:
  friend class ezExpressionCompiler;

  ezDynamicArray<StorageType> m_ByteCode;

  ezUInt32 m_uiNumInstructions;
  ezUInt32 m_uiNumTempRegisters;
};

#include <ProceduralPlacementPlugin/VM/Implementation/ExpressionByteCode_inl.h>
