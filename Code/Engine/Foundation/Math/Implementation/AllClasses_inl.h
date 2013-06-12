#pragma once

#include <Foundation/Math/BoundingBox.h>
#include <Foundation/Math/BoundingSphere.h>
#include <Foundation/Math/Plane.h>
#include <Foundation/Math/Mat3.h>
#include <Foundation/Math/Mat4.h>

template<typename Type>
EZ_FORCE_INLINE bool ezBoundingBoxTemplate<Type>::Contains(const ezBoundingSphereTemplate<Type>& sphere) const
{
  return Contains(sphere.GetBoundingBox());
}

template<typename Type>
EZ_FORCE_INLINE bool ezBoundingBoxTemplate<Type>::Overlaps(const ezBoundingSphereTemplate<Type>& sphere) const
{
  // check whether the closest point between box and sphere is inside the sphere (it is definitely inside the box)
  return sphere.Contains(GetClampedPoint(sphere.m_vCenter));
}

template<typename Type>
inline Type ezBoundingBoxTemplate<Type>::GetDistanceTo(const ezBoundingSphereTemplate<Type>& sphere) const
{
  return (GetClampedPoint(sphere.m_vCenter) - sphere.m_vCenter).GetLength() - sphere.m_fRadius;
}

template<typename Type>
inline const ezBoundingSphereTemplate<Type> ezBoundingBoxTemplate<Type>::GetBoundingSphere() const
{
  return ezBoundingSphereTemplate<Type>(GetCenter(), (m_vMax - m_vMin).GetLength() * (Type) 0.5);
}

template<typename Type>
void ezBoundingSphereTemplate<Type>::ExpandToInclude(const ezBoundingBoxTemplate<Type>& rhs)
{
  // This could be made more efficient by chosing only the point that is farthest away from the sphere

  ezVec3Template<Type> vCorners[8];
  rhs.GetCorners(vCorners);

  ExpandToInclude(vCorners, 8);
}

template<typename Type>
Type ezBoundingSphereTemplate<Type>::GetDistanceTo(const ezBoundingBoxTemplate<Type>& rhs) const
{
  const ezVec3Template<Type> vPointOnBox = rhs.GetClampedPoint(m_vCenter);

  return GetDistanceTo(vPointOnBox);
}

template<typename Type>
bool ezBoundingSphereTemplate<Type>::Contains(const ezBoundingBoxTemplate<Type>& rhs) const
{
  /// \todo This could be done more efficiently.

  ezVec3Template<Type> vCorners[8];
  rhs.GetCorners(vCorners);

  return Contains(vCorners, 8);
}

template<typename Type>
bool ezBoundingSphereTemplate<Type>::Overlaps(const ezBoundingBoxTemplate<Type>& rhs) const
{
  return Contains(rhs.GetClampedPoint(m_vCenter));
}

template<typename Type>
const ezBoundingBoxTemplate<Type> ezBoundingSphereTemplate<Type>::GetBoundingBox() const
{
  return ezBoundingBoxTemplate<Type>(m_vCenter - ezVec3Template<Type>(m_fRadius), m_vCenter + ezVec3Template<Type>(m_fRadius));
}


template<typename Type>
ezPositionOnPlane::Enum ezPlaneTemplate<Type>::GetObjectPosition(const ezBoundingSphereTemplate<Type>& Sphere) const
{
  const Type fDist = GetDistanceTo(Sphere.m_vCenter);

  if (fDist >= Sphere.m_fRadius)
    return ezPositionOnPlane::Front;

  if (-fDist >= Sphere.m_fRadius)
    return ezPositionOnPlane::Back;

  return ezPositionOnPlane::Spanning;
}

template<typename Type>
ezPositionOnPlane::Enum ezPlaneTemplate<Type>::GetObjectPosition(const ezBoundingBoxTemplate<Type>& Box) const
{
  ezVec3Template<Type> vPos = Box.m_vMin;
  ezVec3Template<Type> vNeg = Box.m_vMax;

  if (m_vNormal.x >= (Type) 0)
  {
    vPos.x = Box.m_vMax.x;
    vNeg.x = Box.m_vMin.x;
  }

  if (m_vNormal.y >= (Type) 0)
  {
    vPos.y = Box.m_vMax.y;
    vNeg.y = Box.m_vMin.y;
  }

  if (m_vNormal.z >= (Type) 0)
  {
    vPos.z = Box.m_vMax.z;
    vNeg.z = Box.m_vMin.z;
  }

  if (GetDistanceTo(vPos) <= (Type) 0)
    return ezPositionOnPlane::Back;

  if (GetDistanceTo(vNeg) >= (Type) 0)
    return ezPositionOnPlane::Front;

  return ezPositionOnPlane::Spanning;
}




template<typename Type>
void ezMat3Template<Type>::SetRotationMatrix (const ezVec3Template<Type>& vAxis, Type fAngle)
{
  EZ_ASSERT(vAxis.IsNormalized(), "vAxis must be normalized.");

  const Type rad = ezMath::DegToRad(fAngle);
  const Type cos = ezMath::CosRad (rad);
  const Type sin = ezMath::SinRad (rad);
  const Type oneminuscos = (Type) 1 - cos;

  const Type xy = vAxis.x * vAxis.y;
  const Type xz = vAxis.x * vAxis.z;
  const Type yz = vAxis.y * vAxis.z;

  const Type xsin = vAxis.x * sin;
  const Type ysin = vAxis.y * sin;
  const Type zsin = vAxis.z * sin;

  const Type onecos_xy = oneminuscos * xy;
  const Type onecos_xz = oneminuscos * xz;
  const Type onecos_yz = oneminuscos * yz;

  //Column 1
  Element(0, 0) = cos + (oneminuscos * (vAxis.x * vAxis.x));
  Element(0, 1) = onecos_xy + zsin;
  Element(0, 2) = onecos_xz - ysin;

  //Column 2  )
  Element(1, 0) = onecos_xy - zsin;
  Element(1, 1) = cos + (oneminuscos * (vAxis.y * vAxis.y));
  Element(1, 2) = onecos_yz + xsin;

  //Column 3  )
  Element(2, 0) = onecos_xz + ysin;
  Element(2, 1) = onecos_yz - xsin;
  Element(2, 2) = cos + (oneminuscos * (vAxis.z * vAxis.z));
}

template<typename Type>
ezResult ezMat3Template<Type>::Invert(Type fEpsilon)
{
  const Type fDet = Element(0, 0) * (Element(2, 2) * Element(1, 1) - Element(1, 2) * Element(2, 1)) -
                     Element(0, 1) * (Element(2, 2) * Element(1, 0) - Element(1, 2) * Element(2, 0)) +
                     Element(0, 2) * (Element(2, 1) * Element(1, 0) - Element(1, 1) * Element(2, 0));

  if (ezMath::IsZero (fDet, fEpsilon))
    return EZ_FAILURE;

  const Type fOneDivDet = (Type) 1 / fDet;

  ezMat3Template<Type> Inverse;

  Inverse.Element(0, 0) = (Element(2, 2) * Element(1, 1) - Element(1, 2) * Element(2, 1));
  Inverse.Element(0, 1) =-(Element(2, 2) * Element(0, 1) - Element(0, 2) * Element(2, 1));
  Inverse.Element(0, 2) = (Element(1, 2) * Element(0, 1) - Element(0, 2) * Element(1, 1));

  Inverse.Element(1, 0) =-(Element(2, 2) * Element(1, 0) - Element(1, 2) * Element(2, 0));
  Inverse.Element(1, 1) = (Element(2, 2) * Element(0, 0) - Element(0, 2) * Element(2, 0));
  Inverse.Element(1, 2) =-(Element(1, 2) * Element(0, 0) - Element(0, 2) * Element(1, 0));

  Inverse.Element(2, 0) = (Element(2, 1) * Element(1, 0) - Element(1, 1) * Element(2, 0));
  Inverse.Element(2, 1) =-(Element(2, 1) * Element(0, 0) - Element(0, 1) * Element(2, 0));
  Inverse.Element(2, 2) = (Element(1, 1) * Element(0, 0) - Element(0, 1) * Element(1, 0));

  *this = Inverse * fOneDivDet;
  return EZ_SUCCESS;
}

template<typename Type>
void ezMat3Template<Type>::SetLookInDirectionMatrix (ezVec3Template<Type> vLookDir, ezVec3Template<Type> vUpDir)
{
  EZ_ASSERT(!vLookDir.IsZero(),"The look direction must not be zero.");
  EZ_ASSERT(vUpDir.IsNormalized(), "The up-direction must be normalized.");

  vLookDir.NormalizeIfNotZero();

  if (ezMath::Abs(vLookDir.Dot(vUpDir)) > (Type) 0.9999) // less than 1 degree difference -> problem
    vUpDir = vLookDir.GetOrthogonalVector();

  // ensure vUpDir is orthogonal to the looking direction
  const ezVec3Template<Type> vRightDir = vLookDir.Cross(vUpDir).GetNormalized ();

  // orthogonalize vUpDir
  vUpDir = vRightDir.Cross(vLookDir); // vLookDir and vRightDir are normalized and orthogonal, vUpDir will be normalized too

  SetColumn(0, vRightDir);
  SetColumn(1, vUpDir);
  SetColumn(2, vLookDir);

  /// \todo At some point we need to define a default coordinate system to use.
  /// I don't really know whether this code works (or better: how).
  /// It should probably use an OpenGL or DirectX coordinate system, not sure what exactly.
  /// The first guy to use it, will find it out.
}




template<typename Type>
void ezMat4Template<Type>::SetRotationMatrix(const ezVec3Template<Type>& vAxis, Type fAngle)
{
  EZ_ASSERT(vAxis.IsNormalized(), "vAxis must be normalized.");

  const Type rad = ezMath::DegToRad(fAngle);
  const Type cos = ezMath::CosRad (rad);
  const Type sin = ezMath::SinRad (rad);
  const Type oneminuscos = (Type) 1 - cos;

  const Type xy = vAxis.x * vAxis.y;
  const Type xz = vAxis.x * vAxis.z;
  const Type yz = vAxis.y * vAxis.z;

  const Type xsin = vAxis.x * sin;
  const Type ysin = vAxis.y * sin;
  const Type zsin = vAxis.z * sin;

  const Type onecos_xy = oneminuscos * xy;
  const Type onecos_xz = oneminuscos * xz;
  const Type onecos_yz = oneminuscos * yz;

  //Column 1
  Element(0, 0) = cos + (oneminuscos * (vAxis.x * vAxis.x));
  Element(0, 1) = onecos_xy + zsin;
  Element(0, 2) = onecos_xz - ysin;
  Element(0, 3) = 0;

  //Column 2
  Element(1, 0) = onecos_xy - zsin;
  Element(1, 1) = cos + (oneminuscos * (vAxis.y * vAxis.y));
  Element(1, 2) = onecos_yz + xsin;
  Element(1, 3) = 0;

  //Column 3
  Element(2, 0) = onecos_xz + ysin;
  Element(2, 1) = onecos_yz - xsin;
  Element(2, 2) = cos + (oneminuscos * (vAxis.z * vAxis.z));
  Element(2, 3) = 0;

  //Column 4
  Element(3, 0) = 0;
  Element(3, 1) = 0;
  Element(3, 2) = 0;
  Element(3, 3) = 1;
}

template<typename Type>
ezResult ezMat4Template<Type>::Invert(Type fEpsilon)
{
  ezMat4Template<Type> Inverse;

  const Type fDet = GetDeterminantOf4x4Matrix (*this);

  if (ezMath::IsZero(fDet, fEpsilon))
    return EZ_FAILURE;

  Type fOneDivDet = ezMath::Invert(fDet);

  for (ezInt32 i = 0; i < 4; ++i)
  {
      
    Inverse.Element(i, 0) = GetDeterminantOf3x3SubMatrix (*this, i, 0) * fOneDivDet;
    fOneDivDet = -fOneDivDet;
    Inverse.Element(i, 1) = GetDeterminantOf3x3SubMatrix (*this, i, 1) * fOneDivDet;
    fOneDivDet = -fOneDivDet;
    Inverse.Element(i, 2) = GetDeterminantOf3x3SubMatrix (*this, i, 2) * fOneDivDet;
    fOneDivDet = -fOneDivDet;
    Inverse.Element(i, 3) = GetDeterminantOf3x3SubMatrix (*this, i, 3) * fOneDivDet;
  }

  *this = Inverse;
  return EZ_SUCCESS;
}

template<typename Type>
void ezMat4Template<Type>::SetPerspectiveProjectionMatrixFromFovX (Type fFieldOfViewX, Type fAspectRatioWidthDivHeight, Type fNearZ, Type fFarZ, ezProjectionDepthRange::Enum DepthRange)
{
  const Type xm = fNearZ * ezMath::TanDeg (fFieldOfViewX * (Type) 0.5);
  const Type ym = xm / fAspectRatioWidthDivHeight;

  SetPerspectiveProjectionMatrix (-xm, xm, -ym, ym, fNearZ, fFarZ, DepthRange);
}

template<typename Type>
void ezMat4Template<Type>::SetPerspectiveProjectionMatrixFromFovY (Type fFieldOfViewY, Type fAspectRatioWidthDivHeight, Type fNearZ, Type fFarZ, ezProjectionDepthRange::Enum DepthRange)
{
  const Type ym = fNearZ * ezMath::TanDeg (fFieldOfViewY * (Type) 0.5);
  const Type xm = ym * fAspectRatioWidthDivHeight;

  SetPerspectiveProjectionMatrix (-xm, xm, -ym, ym, fNearZ, fFarZ, DepthRange);
}

template<typename Type>
void ezMat4Template<Type>::SetPerspectiveProjectionMatrix (Type fViewWidth, Type fViewHeight, Type fNearZ, Type fFarZ, ezProjectionDepthRange::Enum DepthRange)
{
  SetPerspectiveProjectionMatrix (-fViewWidth * (Type) 0.5, fViewWidth * (Type) 0.5, -fViewHeight * (Type) 0.5, fViewHeight * (Type) 0.5, fNearZ, fFarZ, DepthRange);
}

template<typename Type>
void ezMat4Template<Type>::SetPerspectiveProjectionMatrix (Type fLeft, Type fRight, Type fBottom, Type fTop, Type fNearZ, Type fFarZ, ezProjectionDepthRange::Enum DepthRange)
{
  const Type fTwoNearZ = fNearZ + fNearZ;
  const Type fOneDivNearMinusFar = (Type) 1 / (fNearZ - fFarZ);

  if (DepthRange == ezProjectionDepthRange::MinusOneToOne)
  {
    Element(2, 2) = -(fFarZ + fNearZ) * fOneDivNearMinusFar;
    Element(3, 2) = 2 * fFarZ * fNearZ * fOneDivNearMinusFar;
  }
  else
  {
    Element(2, 2) = -(fFarZ +    0  ) * fOneDivNearMinusFar;
    Element(3, 2) =    fFarZ * fNearZ * fOneDivNearMinusFar;
  }

  Element(0, 0) = fTwoNearZ / (fRight - fLeft);
  Element(1, 0) = (Type) 0;
  Element(2, 0) = (fLeft + fRight) / (fLeft - fRight);
  Element(3, 0) = (Type) 0;

  Element(0, 1) = (Type) 0;
  Element(1, 1) = fTwoNearZ / (fTop - fBottom);
  Element(2, 1) = (fBottom + fTop) / (fBottom - fTop);
  Element(3, 1) = (Type) 0;

  Element(0, 2) = (Type) 0;
  Element(1, 2) = (Type) 0;

  Element(0, 3) = (Type) 0;
  Element(1, 3) = (Type) 0;
  Element(2, 3) = (Type) 1;
  Element(3, 3) = (Type) 0;
}

template<typename Type>
void ezMat4Template<Type>::SetOrthographicProjectionMatrix(Type fViewWidth, Type fViewHeight, Type fNearZ, Type fFarZ, ezProjectionDepthRange::Enum DepthRange)
{
  SetOrthographicProjectionMatrix(-fViewWidth * (Type) 0.5, fViewWidth * (Type) 0.5, -fViewHeight * (Type) 0.5, fViewHeight * (Type) 0.5, fNearZ, fFarZ, DepthRange);
}

template<typename Type>
void ezMat4Template<Type>::SetOrthographicProjectionMatrix(Type fLeft, Type fRight, Type fBottom, Type fTop, Type fNearZ, Type fFarZ, ezProjectionDepthRange::Enum DepthRange)
{
  const Type fOneDivFarMinusNear = (Type) 1 / (fFarZ - fNearZ);

  if (DepthRange == ezProjectionDepthRange::MinusOneToOne)
  {
    Element(2, 2) =  2 * fOneDivFarMinusNear;
    Element(3, 2) = -(fFarZ + fNearZ) * fOneDivFarMinusNear;
  }
  else
  {
    Element(2, 2) =      fOneDivFarMinusNear;
    Element(3, 2) = -(  0   + fNearZ) * fOneDivFarMinusNear;
  }

  Element(0, 0) = (Type) 2 / (fRight - fLeft);
  Element(1, 0) = (Type) 0;
  Element(2, 0) = (Type) 0;
  Element(3, 0) = (fRight + fLeft) / (fRight - fLeft);

  Element(0, 1) = (Type) 0;
  Element(1, 1) = (Type) 2 / (fTop - fBottom);
  Element(2, 1) = (Type) 0;
  Element(3, 1) = (fTop + fBottom) / (fTop - fBottom);

  Element(0, 2) = (Type) 0;
  Element(1, 2) = (Type) 0;


  Element(0, 3) = (Type) 0;
  Element(1, 3) = (Type) 0;
  Element(2, 3) = (Type) 0;
  Element(3, 3) = (Type) 1;
}

template<typename Type>
void ezMat4Template<Type>::SetLookAtMatrix(const ezVec3Template<Type>& vStartPos, const ezVec3Template<Type>& vTargetPos, const ezVec3Template<Type>& vUpDir)
{
  ezMat3Template<Type> Rotation;
  Rotation.SetLookInDirectionMatrix (vTargetPos - vStartPos, vUpDir);

  SetRotationalPart(Rotation);
  SetTranslationVector(vStartPos);
  SetRow(3, ezVec4Template<Type>(0, 0, 0, 1));
}