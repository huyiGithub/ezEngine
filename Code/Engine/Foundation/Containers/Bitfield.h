#pragma once

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/IO/Stream.h>
#include <Foundation/Math/Constants.h>

/// \brief A template interface, that turns any array class into a bitfield.
///
/// This class provides an interface to work with single bits, to store true/false values.
/// The underlying container is configurable, though it must support random access and a 'SetCount' function and it must use elements of type
/// ezUInt32. In most cases a dynamic array should be used. For this case the ezDynamicBitfield typedef is already available. There is also an
/// ezHybridBitfield typedef.
template <class Container>
class ezBitfield
{
public:
  ezBitfield() = default;

  /// \brief Returns the number of bits that this bitfield stores.
  ezUInt32 GetCount() const; // [tested]

  /// \brief Resizes the Bitfield to hold the given number of bits. This version does NOT initialize new bits!
  template <typename = void>                       // Template is used to only conditionally compile this function in when it is actually used.
  void SetCountUninitialized(ezUInt32 uiBitCount); // [tested]

  /// \brief Resizes the Bitfield to hold the given number of bits. If \a bSetNew is true, new bits are set to 1, otherwise they are cleared to 0.
  void SetCount(ezUInt32 uiBitCount, bool bSetNew = false); // [tested]

  /// \brief Returns true, if the bitfield does not store any bits.
  bool IsEmpty() const; // [tested]

  /// \brief Returns true, if the bitfield is not empty and any bit is 1.
  bool IsAnyBitSet(ezUInt32 uiFirstBit = 0, ezUInt32 uiNumBits = 0xFFFFFFFF) const; // [tested]

  /// \brief Returns true, if the bitfield is empty or all bits are set to zero.
  bool IsNoBitSet(ezUInt32 uiFirstBit = 0, ezUInt32 uiNumBits = 0xFFFFFFFF) const; // [tested]

  /// \brief Discards all bits and sets count to zero.
  void Clear(); // [tested]

  /// \brief Sets the given bit to 1.
  void SetBit(ezUInt32 uiBit); // [tested]

  /// \brief Clears the given bit to 0.
  void ClearBit(ezUInt32 uiBit); // [tested]

  /// \brief Returns true, if the given bit is set to 1.
  bool IsBitSet(ezUInt32 uiBit) const; // [tested]

  /// \brief Clears all bits to 0.
  void ClearAllBits(); // [tested]

  /// \brief Sets all bits to 1.
  void SetAllBits(); // [tested]

  /// \brief Sets the range starting at uiFirstBit up to (and including) uiLastBit to 1.
  void SetBitRange(ezUInt32 uiFirstBit, ezUInt32 uiNumBits); // [tested]

  /// \brief Clears the range starting at uiFirstBit up to (and including) uiLastBit to 0.
  void ClearBitRange(ezUInt32 uiFirstBit, ezUInt32 uiNumBits); // [tested]

private:
  ezUInt32 GetBitInt(ezUInt32 uiBitIndex) const;
  ezUInt32 GetBitMask(ezUInt32 uiBitIndex) const;

  ezUInt32 m_uiCount = 0;
  Container m_Container;
};

/// \brief This should be the main type of bitfield to use, although other internal container types are possible.
using ezDynamicBitfield = ezBitfield<ezDynamicArray<ezUInt32>>;

/// \brief An ezBitfield that uses a hybrid array as internal container.
template <ezUInt32 Size>
using ezHybridBitfield = ezBitfield<ezHybridArray<ezUInt32, Size>>;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template <typename T>
class ezStaticBitfield
{
public:
  using StorageType = T;
  static constexpr ezUInt32 GetNumBits() { return ezMath::NumBits<T>(); }

  /// \brief Initializes the bitfield to all zero.
  ezStaticBitfield();

  static ezStaticBitfield<T> FromMask(StorageType bits);

  /// \brief Returns true, if the bitfield is not zero.
  bool IsAnyBitSet() const; // [tested]

  /// \brief Returns true, if the bitfield is all zero.
  bool IsNoBitSet() const; // [tested]

  /// \brief Sets the given bit to 1.
  void SetBit(ezUInt32 uiBit); // [tested]

  /// \brief Clears the given bit to 0.
  void ClearBit(ezUInt32 uiBit); // [tested]

  /// \brief Returns true, if the given bit is set to 1.
  bool IsBitSet(ezUInt32 uiBit) const; // [tested]

  /// \brief Clears all bits to 0. Same as Clear().
  void ClearAllBits(); // [tested]

  /// \brief Sets all bits to 1.
  void SetAllBits(); // [tested]

  /// \brief Sets the range starting at uiFirstBit up to (and including) uiLastBit to 1.
  void SetBitRange(ezUInt32 uiFirstBit, ezUInt32 uiNumBits); // [tested]

  /// \brief Clears the range starting at uiFirstBit up to (and including) uiLastBit to 0.
  void ClearBitRange(ezUInt32 uiFirstBit, ezUInt32 uiNumBits); // [tested]

  /// \brief Returns the raw uint that stores all bits.
  T GetValue() const; // [tested]

  /// \brief Sets the raw uint that stores all bits.
  void SetValue(T value); // [tested]

  /// \brief Modifies \a this to also contain the bits from \a rhs.
  EZ_ALWAYS_INLINE void operator|=(const ezStaticBitfield<T>& rhs) { m_Storage |= rhs.m_Storage; }

  /// \brief Modifies \a this to only contain the bits that were set in \a this and \a rhs.
  EZ_ALWAYS_INLINE void operator&=(const ezStaticBitfield<T>& rhs) { m_Storage &= rhs.m_Storage; }

  ezResult Serialize(ezStreamWriter& writer) const
  {
    writer.WriteVersion(s_Version);
    writer << m_Storage;
    return EZ_SUCCESS;
  }

  ezResult Deserialize(ezStreamReader& reader)
  {
    /*auto version =*/reader.ReadVersion(s_Version);
    reader >> m_Storage;
    return EZ_SUCCESS;
  }

private:
  static constexpr ezTypeVersion s_Version = 1;

  ezStaticBitfield(StorageType initValue)
    : m_Storage(initValue)
  {
  }

  template <typename U>
  friend ezStaticBitfield<U> operator|(ezStaticBitfield<U> lhs, ezStaticBitfield<U> rhs);

  template <typename U>
  friend ezStaticBitfield<U> operator&(ezStaticBitfield<U> lhs, ezStaticBitfield<U> rhs);

  template <typename U>
  friend ezStaticBitfield<U> operator^(ezStaticBitfield<U> lhs, ezStaticBitfield<U> rhs);

  template <typename U>
  friend bool operator==(ezStaticBitfield<U> lhs, ezStaticBitfield<U> rhs);

  template <typename U>
  friend bool operator!=(ezStaticBitfield<U> lhs, ezStaticBitfield<U> rhs);

  StorageType m_Storage = 0;
};

template <typename T>
inline ezStaticBitfield<T> operator|(ezStaticBitfield<T> lhs, ezStaticBitfield<T> rhs)
{
  return ezStaticBitfield<T>(lhs.m_Storage | rhs.m_Storage);
}

template <typename T>
inline ezStaticBitfield<T> operator&(ezStaticBitfield<T> lhs, ezStaticBitfield<T> rhs)
{
  return ezStaticBitfield<T>(lhs.m_Storage & rhs.m_Storage);
}

template <typename T>
inline ezStaticBitfield<T> operator^(ezStaticBitfield<T> lhs, ezStaticBitfield<T> rhs)
{
  return ezStaticBitfield<T>(lhs.m_Storage ^ rhs.m_Storage);
}

template <typename T>
inline bool operator==(ezStaticBitfield<T> lhs, ezStaticBitfield<T> rhs)
{
  return lhs.m_Storage == rhs.m_Storage;
}

template <typename T>
inline bool operator!=(ezStaticBitfield<T> lhs, ezStaticBitfield<T> rhs)
{
  return lhs.m_Storage != rhs.m_Storage;
}

using ezStaticBitfield8 = ezStaticBitfield<ezUInt8>;
using ezStaticBitfield16 = ezStaticBitfield<ezUInt16>;
using ezStaticBitfield32 = ezStaticBitfield<ezUInt32>;
using ezStaticBitfield64 = ezStaticBitfield<ezUInt64>;

#include <Foundation/Containers/Implementation/Bitfield_inl.h>
