#include <PCH.h>
#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>
#include <Foundation/Math/Vec4.h>

#include <Foundation/Math/FixedPoint.h>

EZ_CREATE_SIMPLE_TEST(Math, Vec2)
{
  EZ_TEST_BLOCK(true, "Constructor")
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
    if (ezMath::BasicType<ezMathTestType>::SupportsNaN())
    {
      // In debug the default constructor initializes everything with NaN.
      ezVec2T vDefCtor;
      EZ_TEST(ezMath::IsNaN(vDefCtor.x) && ezMath::IsNaN(vDefCtor.y));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    ezVec2T::ComponentType testBlock[2] = { (ezVec2T::ComponentType) 1, (ezVec2T::ComponentType) 2 };
    ezVec2T* pDefCtor = ::new ((void*)&testBlock[0]) ezVec2T;
    EZ_TEST(pDefCtor->x == (ezVec2T::ComponentType) 1 && pDefCtor->y == (ezVec2T::ComponentType) 2);
#endif
  }

  EZ_TEST_BLOCK(true, "Constructor(x,y)")
  {
    ezVec2T v(1, 2);
    EZ_TEST_FLOAT(v.x, 1, 0);
    EZ_TEST_FLOAT(v.y, 2, 0);
  }

  EZ_TEST_BLOCK(true, "Constructor(xy)")
  {
    ezVec2T v(3);
    EZ_TEST_VEC2(v, ezVec2T(3, 3), 0);
  }

  EZ_TEST_BLOCK(true, "ZeroVector")
  {
    EZ_TEST_VEC2(ezVec2T::ZeroVector(), ezVec2T(0, 0), 0);
  }

  EZ_TEST_BLOCK(true, "GetAsVec3")
  {
    EZ_TEST_VEC3(ezVec2T(2, 3).GetAsVec3(4), ezVec3T(2, 3, 4), 0);
  }

  EZ_TEST_BLOCK(true, "GetAsVec4")
  {
    EZ_TEST_VEC4(ezVec2T(2, 3).GetAsVec4(4, 5), ezVec4T(2, 3, 4, 5), 0);
  }

  EZ_TEST_BLOCK(true, "Set(x, y)")
  {
    ezVec2T v;
    v.Set(2, 3);

    EZ_TEST_FLOAT(v.x, 2, 0);
    EZ_TEST_FLOAT(v.y, 3, 0);
  }

  EZ_TEST_BLOCK(true, "Set(xy)")
  {
    ezVec2T v;
    v.Set(4);

    EZ_TEST_FLOAT(v.x, 4, 0);
    EZ_TEST_FLOAT(v.y, 4, 0);
  }

  EZ_TEST_BLOCK(true, "SetZero")
  {
    ezVec2T v;
    v.Set(4);
    v.SetZero();

    EZ_TEST_FLOAT(v.x, 0, 0);
    EZ_TEST_FLOAT(v.y, 0, 0);
  }

  EZ_TEST_BLOCK(true, "GetLength")
  {
    ezVec2T v(0);
    EZ_TEST_FLOAT(v.GetLength(), 0, 0.0001f);

    v.Set(1, 0);
    EZ_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(0, 1);
    EZ_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(2, 3);
    EZ_TEST_FLOAT(v.GetLength(), ezMath::Sqrt((ezMathTestType) (4 + 9)), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "GetLengthSquared")
  {
    ezVec2T v(0);
    EZ_TEST_FLOAT(v.GetLengthSquared(), 0, 0.0001f);

    v.Set(1, 0);
    EZ_TEST_FLOAT(v.GetLengthSquared(), 1, 0.0001f);

    v.Set(0, 1);
    EZ_TEST_FLOAT(v.GetLengthSquared(), 1, 0.0001f);

    v.Set(2, 3);
    EZ_TEST_FLOAT(v.GetLengthSquared(), 4 + 9, 0.0001f);
  }

  EZ_TEST_BLOCK(true, "GetLengthAndNormalize")
  {
    ezVec2T v(0.5f, 0);
    ezVec2T::ComponentType l = v.GetLengthAndNormalize();
    EZ_TEST_FLOAT(l, 0.5f, 0.0001f);
    EZ_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(1, 0);
    l = v.GetLengthAndNormalize();
    EZ_TEST_FLOAT(l, 1, 0.0001f);
    EZ_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(0, 1);
    l = v.GetLengthAndNormalize();
    EZ_TEST_FLOAT(l, 1, 0.0001f);
    EZ_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(2, 3);
    l = v.GetLengthAndNormalize();
    EZ_TEST_FLOAT(l, ezMath::Sqrt((ezMathTestType) (4 + 9)), 0.0001f);
    EZ_TEST_FLOAT(v.GetLength(), 1, 0.0001f);
  }

  EZ_TEST_BLOCK(true, "GetNormalized")
  {
    ezVec2T v;

    v.Set(10, 0);
    EZ_TEST_VEC2(v.GetNormalized(), ezVec2T(1, 0), 0.001f);

    v.Set(0, 10);
    EZ_TEST_VEC2(v.GetNormalized(), ezVec2T(0, 1), 0.001f);
  }

  EZ_TEST_BLOCK(true, "Normalize")
  {
    ezVec2T v;

    v.Set(10, 0);
    v.Normalize();
    EZ_TEST_VEC2(v, ezVec2T(1, 0), 0.001f);

    v.Set(0, 10);
    v.Normalize();
    EZ_TEST_VEC2(v, ezVec2T(0, 1), 0.001f);
  }

  EZ_TEST_BLOCK(true, "NormalizeIfNotZero")
  {
    ezVec2T v;

    v.Set(10, 0);
    EZ_TEST(v.NormalizeIfNotZero() == EZ_SUCCESS);
    EZ_TEST_VEC2(v, ezVec2T(1, 0), 0.001f);

    v.Set(0, 10);
    EZ_TEST(v.NormalizeIfNotZero() == EZ_SUCCESS);
    EZ_TEST_VEC2(v, ezVec2T(0, 1), 0.001f);

    v.SetZero();
    EZ_TEST(v.NormalizeIfNotZero() == EZ_FAILURE);
    EZ_TEST_VEC2(v, ezVec2T(1, 0), 0.001f);
  }

  EZ_TEST_BLOCK(true, "IsZero")
  {
    ezVec2T v;
    
    v.Set(1);
    EZ_TEST(v.IsZero() == false);

    v.Set(0.001f);
    EZ_TEST(v.IsZero() == false);
    EZ_TEST(v.IsZero(0.01f) == true);
  }

  EZ_TEST_BLOCK(true, "IsNormalized")
  {
    ezVec2T v;

    v.SetZero();
    EZ_TEST(v.IsNormalized(ezMath::BasicType<ezMathTestType>::HugeEpsilon()) == false);

    v.Set(1, 0);
    EZ_TEST(v.IsNormalized(ezMath::BasicType<ezMathTestType>::HugeEpsilon()) == true);

    v.Set(0, 1);
    EZ_TEST(v.IsNormalized(ezMath::BasicType<ezMathTestType>::HugeEpsilon()) == true);

    v.Set(0.1f, 1);
    EZ_TEST(v.IsNormalized(ezMath::BasicType<ezMathTestType>::DefaultEpsilon()) == false);
  }

  EZ_TEST_BLOCK(true, "IsNaN")
  {
    if (ezMath::BasicType<ezMathTestType>::SupportsNaN())
    {
      ezVec2T v(0);

      EZ_TEST(!v.IsNaN());

      v.x = ezMath::BasicType<ezMathTestType>::GetNaN();
      EZ_TEST(v.IsNaN());
    }
  }

  EZ_TEST_BLOCK(true, "IsValid")
  {
    if (ezMath::BasicType<ezMathTestType>::SupportsNaN())
    {
      ezVec2T v(0);

      EZ_TEST(v.IsValid());

      v.x = ezMath::BasicType<ezMathTestType>::GetNaN();
      EZ_TEST(!v.IsValid());
    }
  }

  EZ_TEST_BLOCK(true, "operator-")
  {
    ezVec2T v(1);

    EZ_TEST_VEC2(-v, ezVec2T(-1), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator+=")
  {
    ezVec2T v(1, 2);

    v += ezVec2T(3, 4);
    EZ_TEST_VEC2(v, ezVec2T(4, 6), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator-=")
  {
    ezVec2T v(1, 2);

    v -= ezVec2T(3, 5);
    EZ_TEST_VEC2(v, ezVec2T(-2, -3), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator*=(float)")
  {
    ezVec2T v(1, 2);

    v *= 3;
    EZ_TEST_VEC2(v, ezVec2T(3, 6), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator/=(float)")
  {
    ezVec2T v(1, 2);

    v /= 2;
    EZ_TEST_VEC2(v, ezVec2T(0.5f, 1), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "IsIdentical")
  {
    ezVec2T v1(1, 2);
    ezVec2T v2 = v1;

    EZ_TEST(v1.IsIdentical(v2));

    v2.x += ezVec2T::ComponentType(0.001f);
    EZ_TEST(!v1.IsIdentical(v2));
  }

  EZ_TEST_BLOCK(true, "IsEqual")
  {
    ezVec2T v1(1, 2);
    ezVec2T v2 = v1;

    EZ_TEST(v1.IsEqual(v2, 0.00001f));

    v2.x += ezVec2T::ComponentType(0.001f);
    EZ_TEST(!v1.IsEqual(v2, 0.0001f));
    EZ_TEST(v1.IsEqual(v2, 0.01f));
  }

  EZ_TEST_BLOCK(true, "GetAngleBetween")
  {
    ezVec2T v1(1, 0);
    ezVec2T v2(0, 1);

    EZ_TEST_FLOAT(v1.GetAngleBetween(v1),  0, 0.001f);
    EZ_TEST_FLOAT(v2.GetAngleBetween(v2),  0, 0.001f);
    EZ_TEST_FLOAT(v1.GetAngleBetween(v2),   90, 0.001f);
    EZ_TEST_FLOAT(v1.GetAngleBetween(-v1), 180, 0.001f);
  }

  EZ_TEST_BLOCK(true, "Dot")
  {
    ezVec2T v1(1, 0);
    ezVec2T v2(0, 1);
    ezVec2T v0(0, 0);

    EZ_TEST_FLOAT(v0.Dot(v0),   0, 0.001f);
    EZ_TEST_FLOAT(v1.Dot(v1),   1, 0.001f);
    EZ_TEST_FLOAT(v2.Dot(v2),   1, 0.001f);
    EZ_TEST_FLOAT(v1.Dot(v2),   0, 0.001f);
    EZ_TEST_FLOAT(v1.Dot(-v1), -1, 0.001f);
  }

  EZ_TEST_BLOCK(true, "CompMin")
  {
    ezVec2T v1(2, 3);
    ezVec2T v2 = v1.CompMin(ezVec2T(1, 4));
    EZ_TEST_VEC2(v2, ezVec2T(1, 3), 0);

    v2 = v1.CompMin(ezVec2T(3, 1));
    EZ_TEST_VEC2(v2, ezVec2T(2, 1), 0);
  }

  EZ_TEST_BLOCK(true, "CompMax")
  {
    ezVec2T v1(2, 3.5f);
    ezVec2T v2 = v1.CompMax(ezVec2T(1, 4));
    EZ_TEST_VEC2(v2, ezVec2T(2, 4), 0);

    v2 = v1.CompMax(ezVec2T(3, 1));
    EZ_TEST_VEC2(v2, ezVec2T(3, 3.5f), 0);
  }

  EZ_TEST_BLOCK(true, "CompMult")
  {
    ezVec2T v1(2, 3);
    ezVec2T v2 = v1.CompMult(ezVec2T(2, 4));
    EZ_TEST_VEC2(v2, ezVec2T(4, 12), 0);

    v2 = v1.CompMult(ezVec2T(3, 7));
    EZ_TEST_VEC2(v2, ezVec2T(6, 21), 0);
  }

  EZ_TEST_BLOCK(true, "CompDiv")
  {
    ezVec2T v1(12, 32);
    ezVec2T v2 = v1.CompDiv(ezVec2T(3, 4));
    EZ_TEST_VEC2(v2, ezVec2T(4, 8), 0);
  }

  EZ_TEST_BLOCK(true, "MakeOrthogonalTo")
  {
    ezVec2T v;
    
    v.Set(1, 1);
    v.MakeOrthogonalTo(ezVec2T(1, 0));
    EZ_TEST_VEC2(v, ezVec2T(0, 1), 0.001f);

    v.Set(1, 1);
    v.MakeOrthogonalTo(ezVec2T(0, 1));
    EZ_TEST_VEC2(v, ezVec2T(1, 0), 0.001f);
  }

  EZ_TEST_BLOCK(true, "GetOrthogonalVector")
  {
    ezVec2T v;
   
    for (float i = 1; i < 360; i += 3)
    {
      v.Set(i, i * 3);
      EZ_TEST_FLOAT(v.GetOrthogonalVector().Dot(v), 0, 0.001f);
    }
  }

  EZ_TEST_BLOCK(true, "GetReflectedVector")
  {
    ezVec2T v, v2;

    v.Set(1, 1);
    v2 = v.GetReflectedVector(ezVec2T(0, -1));
    EZ_TEST_VEC2(v2, ezVec2T(1, -1), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator+")
  {
    ezVec2T v = ezVec2T(1, 2) + ezVec2T(3, 4);
    EZ_TEST_VEC2(v, ezVec2T(4, 6), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator-")
  {
    ezVec2T v = ezVec2T(1, 2) - ezVec2T(3, 5);
    EZ_TEST_VEC2(v, ezVec2T(-2, -3), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator* (vec, float) | operator* (float, vec)")
  {
    ezVec2T v = ezVec2T(1, 2) * ezVec2T::ComponentType(3);
    EZ_TEST_VEC2(v, ezVec2T(3, 6), 0.0001f);

    v = ezVec2T::ComponentType(7) * ezVec2T(1, 2);
    EZ_TEST_VEC2(v, ezVec2T(7, 14), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator/ (vec, float)")
  {
    ezVec2T v = ezVec2T(2, 4) / ezVec2T::ComponentType(2);
    EZ_TEST_VEC2(v, ezVec2T(1, 2), 0.0001f);
  }

  EZ_TEST_BLOCK(true, "operator== | operator!=")
  {
    ezVec2T v1(1, 2);
    ezVec2T v2 = v1;

    EZ_TEST(v1 == v2);

    v2.x += ezVec2T::ComponentType(0.001f);
    EZ_TEST(v1 != v2);
  }
}