/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_nsAStringOrJSString_h_
#define mozilla_dom_nsAStringOrJSString_h_

#include "js/String.h"
#include "nsAString.h"
#include "nsJSUtils.h"

namespace mozilla::dom {

#if 0

class nsAStringOrJSString {
 public:
  MOZ_IMPLICIT nsAStringOrJSString(const nsAString& aString)
      : mXPCOMStringOrJSContext((void*)&aString), mJSString(nullptr) {}
  nsAStringOrJSString(JSContext* aCx, JS::Handle<JSString*> aString)
      : mXPCOMStringOrJSContext((void*)aCx), mJSString(aString) {
    MOZ_ASSERT(aString);
    MOZ_ASSERT(aCx);
  }

  size_t Length() const {
    if (mJSString) {
      return JS_GetStringLength(mJSString);
    }
    return ((nsAString*)mXPCOMStringOrJSContext)->Length();
  }

  void AssignTo(nsAString& aOut) const {
    if (mJSString) {
      bool ok =
          AssignJSString((JSContext*)mXPCOMStringOrJSContext, aOut, mJSString);
      MOZ_RELEASE_ASSERT(ok);
      return;
    }
    aOut.Assign(*((nsAString*)mXPCOMStringOrJSContext));
  }

  /**
   * Walks a potentially-rope string and calls `utf16func` for UTF-16 segments
   * and `latin1func` for Latin1 segments. The callbacks are not called for
   * empty segments. The callbacks are allowed to trigger GC.
   *
   * `context` is passed back to the callbacks.
   *
   * Return `false` either on OOM (possible due to internally using a stack) or
   * if a callback returned `false`. Otherwise return `true`.
   */
  bool WalkRope(void* aContext, JS_WalkRopeMayGCUTF16Func* aUtf16func,
                JS_WalkRopeMayGCLatin1Func* aLatin1func) const {
    if (mJSString) {
      JSContext* cx = (JSContext*)mXPCOMStringOrJSContext;
      return JS_WalkRope(cx, mJSString, aContext, aUtf16func, aLatin1func);
    }
    return (*aUtf16func)(aContext, *((nsAString*)mXPCOMStringOrJSContext));
  }

  /**
   * Walks a potentially-rope string and calls `utf16func` for UTF-16 segments
   * and `latin1func` for Latin1 segments. The callbacks are not called for
   * empty segments. The callbacks are MUST NOT trigger GC.
   *
   * `context` is passed back to the callbacks.
   *
   * Return `false` either on OOM (possible due to internally using a stack) or
   * if a callback returned `false`. Otherwise return `true`.
   */
  bool WalkRopeUnsafe(void* aContext,
                      JS_UnsafeWalkRopeMustNotGCUTF16Func* aUtf16func,
                      JS_UnsafeWalkRopeMustNotGCLatin1Func* aLatin1func) const {
    if (mJSString) {
      JSContext* cx = (JSContext*)mXPCOMStringOrJSContext;
      return JS_WalkRopeUnsafe(cx, mJSString, aContext, aUtf16func,
                               aLatin1func);
    }
    return (*aUtf16func)(aContext, *((nsAString*)mXPCOMStringOrJSContext));
  }

 private:
  void* mXPCOMStringOrJSContext;
  JS::Handle<JSString*> mJSString;
};

#endif

class nsAStringOrJSString {
 public:
  MOZ_IMPLICIT nsAStringOrJSString(const nsAString& aString)
      : mUTF16(&aString), mLatin1(nullptr) {}
  MOZ_IMPLICIT nsAStringOrJSString(const nsACString& aString)
      : mUTF16(nullptr), mLatin1(&aString) {}

  size_t Length() const {
    if (mUTF16) {
      return mUTF16->Length();
    }
    return mLatin1->Length();
  }

  bool IsLatin1() const { return !!mLatin1; }

  const nsAString& GetUTF16() const {
    MOZ_ASSERT(mUTF16);
    return *mUTF16;
  }

  const nsACString& GetLatin1() const {
    MOZ_ASSERT(mLatin1);
    return *mLatin1;
  }

  void AssignTo(nsAString& aOut) const {
    if (mUTF16) {
      aOut.Assign(*mUTF16);
      return;
    }
    CopyASCIItoUTF16(*mLatin1, aOut);
  }

  void AssignTo(nsACString& aOut) const {
    if (mLatin1) {
      aOut.Assign(*mLatin1);
      return;
    }
    MOZ_RELEASE_ASSERT(false);
  }

  /**
   * Walks a potentially-rope string and calls `utf16func` for UTF-16 segments
   * and `latin1func` for Latin1 segments. The callbacks are not called for
   * empty segments. The callbacks are allowed to trigger GC.
   *
   * `context` is passed back to the callbacks.
   *
   * Return `false` either on OOM (possible due to internally using a stack) or
   * if a callback returned `false`. Otherwise return `true`.
   */
  bool WalkRope(void* aContext, JS_WalkRopeMayGCUTF16Func* aUtf16func,
                JS_WalkRopeMayGCLatin1Func* aLatin1func) const {
    if (mUTF16) {
      if (mUTF16->IsEmpty()) {
        return true;
      }
      return (*aUtf16func)(aContext, *mUTF16);
    }
    if (mLatin1->IsEmpty()) {
      return true;
    }
    return (*aLatin1func)(aContext, *mLatin1);
  }

  /**
   * Walks a potentially-rope string and calls `utf16func` for UTF-16 segments
   * and `latin1func` for Latin1 segments. The callbacks are not called for
   * empty segments. The callbacks are MUST NOT trigger GC.
   *
   * `context` is passed back to the callbacks.
   *
   * Return `false` either on OOM (possible due to internally using a stack) or
   * if a callback returned `false`. Otherwise return `true`.
   */
  bool WalkRopeUnsafe(void* aContext,
                      JS_UnsafeWalkRopeMustNotGCUTF16Func* aUtf16func,
                      JS_UnsafeWalkRopeMustNotGCLatin1Func* aLatin1func) const {
    if (mUTF16) {
      if (mUTF16->IsEmpty()) {
        return true;
      }
      return (*aUtf16func)(aContext, *mUTF16);
    }
    if (mLatin1->IsEmpty()) {
      return true;
    }
    return (*aLatin1func)(aContext, *mLatin1);
  }

 private:
  const nsAString* mUTF16;
  const nsACString* mLatin1;
};

static_assert(std::is_trivially_copyable_v<nsAStringOrJSString>);

};  // namespace mozilla::dom

#endif  // mozilla_dom_nsAStringOrJSString_h_
