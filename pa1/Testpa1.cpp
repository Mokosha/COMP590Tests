#include "gtest/gtest.h"

#include "GContext.h"
#include "GBitmap.h"
#include "GColor.h"

TEST(GContext, CreateFromBitmap) {
  GBitmap bm;
  bm.fWidth = 64;
  bm.fHeight = 128;
  const int nPixels = bm.fWidth * bm.fHeight;
  bm.fPixels = new GPixel[nPixels];
  bm.fRowBytes = bm.fWidth * sizeof(GPixel);

  memset(bm.fPixels, 0, sizeof(GPixel) * nPixels);

  GContext *newContext = GContext::Create(bm);

  GBitmap nbm;
  newContext->getBitmap(&nbm);

  EXPECT_EQ(memcmp(&bm, &nbm, sizeof(GBitmap)), 0);

  delete newContext;
}

TEST(GContext, CreateFromWidthAndHeight) {
  GContext *newContext = GContext::Create(64, 128);

  GBitmap nbm;
  nbm.fPixels = NULL;

  newContext->getBitmap(&nbm);

  EXPECT_EQ(nbm.fWidth, 64);
  EXPECT_EQ(nbm.fHeight, 128);
  EXPECT_NE(nbm.fPixels, static_cast<GPixel *>(NULL));

  delete newContext;
}

TEST(GContext, ClearOwnBitmap) {
  GContext *ctx = GContext::Create(32, 32);

  GColor clr;
  clr.fA = clr.fR = clr.fG = clr.fB = 1.0f;
  ctx->clear(clr);

  GBitmap bm;
  ctx->getBitmap(&bm);

  EXPECT_EQ(bm.fWidth, 32);
  EXPECT_EQ(bm.fHeight, 32);

  for(int i = 0; i < 32; i++) {
    for(int j = 0; j < 32; j++) {
      EXPECT_EQ(bm.fPixels[j*32 + i], 0xFFFFFFFF);
    }
  }

  delete ctx;
}

TEST(GContext, ClearForeignBitmap) {
  GBitmap bm;
  bm.fWidth = 64;
  bm.fHeight = 128;
  const int nPixels = bm.fWidth * bm.fHeight;
  bm.fPixels = new GPixel[nPixels];
  bm.fRowBytes = bm.fWidth * sizeof(GPixel);

  memset(bm.fPixels, 0, sizeof(GPixel) * nPixels);

  GContext *ctx = GContext::Create(bm);

  GColor clr;
  clr.fA = clr.fR = clr.fG = clr.fB = 1.0f;
  ctx->clear(clr);

  for(int j = 0; j < 128; j++) {
    for(int i = 0; i < 64; i++) {
      const int pIdx = j * bm.fWidth + i;
      EXPECT_EQ(bm.fPixels[pIdx], 0xFFFFFFFF);
    }
  }

  delete ctx;
}

TEST(GContext, ClearBitmapWithSlop) {
  GBitmap bm;
  bm.fWidth = 640;
  bm.fHeight = 12;
  bm.fRowBytes = bm.fWidth * sizeof(GPixel) + 16;
  const int fPixelsSz = bm.fRowBytes * bm.fHeight;
  bm.fPixels = (GPixel *)malloc(fPixelsSz);

  memset(bm.fPixels, 0, fPixelsSz);

  GContext *ctx = GContext::Create(bm);

  GColor clr;
  clr.fA = clr.fR = clr.fG = clr.fB = 1.0f;
  ctx->clear(clr);

  for(int j = 0; j < bm.fHeight; j++) {
    for(int i = 0; i < bm.fWidth; i++) {
      const unsigned char *pixelAddress =
        reinterpret_cast<const unsigned char *>(bm.fPixels);
      pixelAddress += (j * bm.fRowBytes) + (i * sizeof(GPixel));
      GPixel p = *(reinterpret_cast<const GPixel *>(pixelAddress));
      EXPECT_EQ(p, 0xFFFFFFFF);
    }
  }

  delete ctx;
}

// Test to make sure that we round properly
TEST(GContext, ClearBitmapWithBarelyWhite) {
  const int w = 1423;
  const int h = 3;

  GContext *ctx = GContext::Create(w, h);

  GColor clr;
  clr.fA = clr.fR = clr.fG = clr.fB = 0.9999999f;
  ctx->clear(clr);

  GBitmap bm;
  ctx->getBitmap(&bm);

  EXPECT_EQ(bm.fWidth, w);
  EXPECT_EQ(bm.fHeight, h);

  for(int j = 0; j < h; j++) {
    for(int i = 0; i < w; i++) {
      EXPECT_EQ(bm.fPixels[j*w + i], 0xFFFFFFFF);
    }
  }

  delete ctx;
}

// Test to make sure that we premultiply alpha
TEST(GContext, ClearBitmapWithAlpha) {
  const int w = 2;
  const int h = 123;

  GContext *ctx = GContext::Create(w, h);

  GColor clr;
  clr.fA = 0.5;
  clr.fR = 1.0;
  clr.fG = 0.0;
  clr.fB = 0.5;
  ctx->clear(clr);

  GBitmap bm;
  ctx->getBitmap(&bm);

  EXPECT_EQ(bm.fWidth, w);
  EXPECT_EQ(bm.fHeight, h);

  for(int j = 0; j < h; j++) {
    for(int i = 0; i < w; i++) {
      EXPECT_EQ(bm.fPixels[j*w + i], 0x80800040);
    }
  }

  delete ctx;
}

TEST(GContext, InvalidInputs) {
  const int iw = -3;
  const int vw = 3;

  const int ih = -6;
  const int vh = 6;

  EXPECT_EQ(GContext::Create(iw, vh), static_cast<GContext *>(NULL));
  EXPECT_EQ(GContext::Create(0, vh), static_cast<GContext *>(NULL));

  EXPECT_EQ(GContext::Create(vw, ih), static_cast<GContext *>(NULL));
  EXPECT_EQ(GContext::Create(vw, 0), static_cast<GContext *>(NULL));

  EXPECT_EQ(GContext::Create(iw, ih), static_cast<GContext *>(NULL));
  EXPECT_EQ(GContext::Create(0, 0), static_cast<GContext *>(NULL));

  GBitmap ibm;
  ibm.fPixels = NULL;

  EXPECT_EQ(GContext::Create(ibm), static_cast<GContext *>(NULL));
}

TEST(GContext, PersistentBitmap) {
  const int w = 2;
  const int h = 123;

  GContext *ctx = GContext::Create(w, h);

  GBitmap bm;
  ctx->getBitmap(&bm);

  GColor clr;
  clr.fA = clr.fG = clr.fR = clr.fB = 1.0;
  ctx->clear(clr);

  EXPECT_EQ(bm.fWidth, w);
  EXPECT_EQ(bm.fHeight, h);

  for(int j = 0; j < h; j++) {
    for(int i = 0; i < w; i++) {
      EXPECT_EQ(bm.fPixels[j*w + i], 0xFFFFFFFF);
    }
  }

  delete ctx;
}

TEST(GContext, OutOfRangeColors) {
  const int w = 2;
  const int h = 123;

  GContext *ctx = GContext::Create(w, h);

  GBitmap bm;
  ctx->getBitmap(&bm);

  GColor clr;
  clr.fR = clr.fG = -3.2;
  clr.fA = clr.fB = 4.0;
  ctx->clear(clr);

  EXPECT_EQ(bm.fWidth, w);
  EXPECT_EQ(bm.fHeight, h);

  for(int j = 0; j < h; j++) {
    for(int i = 0; i < w; i++) {
      EXPECT_EQ(bm.fPixels[j*w + i], 0xFF0000FF);
    }
  }

  delete ctx;
}

TEST(GContext, CreateFromInvalidBitmap) {
  GContext *ctx;

  GBitmap bm;
  bm.fWidth = 64;
  bm.fHeight = 128;
  const int nPixels = bm.fWidth * bm.fHeight;
  bm.fPixels = new GPixel[nPixels];
  bm.fRowBytes = 0;

  ctx = GContext::Create(bm);
  EXPECT_FALSE(ctx);

  bm.fRowBytes = bm.fWidth * sizeof(GPixel) + 13;

  ctx = GContext::Create(bm);
  EXPECT_FALSE(ctx);

  bm.fRowBytes = bm.fWidth * sizeof(GPixel);
  delete bm.fPixels;
  bm.fPixels = NULL;

  ctx = GContext::Create(bm);
  EXPECT_FALSE(ctx);
}
