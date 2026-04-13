/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

protected:
nsHtml5Buffer(Char* aBuffer, int32_t aEnd)
    : buffer(aBuffer), start(0), end(aEnd) {}
~nsHtml5Buffer() {}

/**
 * For working around the privacy of |buffer| in the generated code.
 */
void DeleteBuffer() { delete[] buffer; }

/**
 * For working around the privacy of |buffer| in the generated code.
 */
void Swap(nsHtml5Buffer<Char>* aOther) {
  Char* tempBuffer = buffer;
  int32_t tempStart = start;
  int32_t tempEnd = end;
  buffer = aOther->buffer;
  start = aOther->start;
  end = aOther->end;
  aOther->buffer = tempBuffer;
  aOther->start = tempStart;
  aOther->end = tempEnd;
}
