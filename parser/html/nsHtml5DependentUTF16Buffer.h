/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsHtml5DependentUTF16Buffer_h
#define nsHtml5DependentUTF16Buffer_h

#include "nscore.h"
#include "nsHtml5OwningUTF16Buffer.h"

template <typename Char>
class MOZ_STACK_CLASS nsHtml5DependentBuffer : public nsHtml5Buffer<Char> {
 public:
  /**
   * Wraps a string without taking ownership of the buffer. aToWrap MUST NOT
   * go away or be shortened while nsHtml5DependentUTF16Buffer is in use.
   */
  explicit nsHtml5DependentBuffer(mozilla::Span<const Char> aToWrap)
      : nsHtml5Buffer<Char>(const_cast<Char*>(aToWrap.Elements()),
                            aToWrap.Length()) {}

  ~nsHtml5DependentBuffer() {};

  /**
   * Copies the currently unconsumed part of this buffer into a new
   * heap-allocated nsHtml5OwningUTF16Buffer. The new object is allocated
   * with a fallible allocator. If the allocation fails, nullptr is returned.
   * @return heap-allocated copy or nullptr if memory allocation failed
   */
  template <typename = std::enable_if<std::is_same_v<Char, char16_t>>>
  already_AddRefed<nsHtml5OwningUTF16Buffer> FalliblyCopyAsOwningBuffer() {
    int32_t newLength = this->getEnd() - this->getStart();
    RefPtr<nsHtml5OwningUTF16Buffer> newObj =
        nsHtml5OwningUTF16Buffer::FalliblyCreate(newLength);
    if (!newObj) {
      return nullptr;
    }
    newObj->setEnd(newLength);
    memcpy(newObj->getBuffer(), this->getBuffer() + this->getStart(),
           newLength * sizeof(char16_t));
    return newObj.forget();
  }
};

using nsHtml5DependentUTF16Buffer = nsHtml5DependentBuffer<char16_t>;

#endif  // nsHtml5DependentUTF16Buffer_h
