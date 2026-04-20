//===- NpuirToTlirConversion.h - NPUIR to TLIR Conversion Pass -*- C++ -*-===//
//
// Copyright (c) Tile-AI Corporation.
// Licensed under the MIT License.
//
//===----------------------------------------------------------------------===//
//
// This file declares the NPUIR (HIVM) to TLIR (Tilelang) conversion pass.
//
//===----------------------------------------------------------------------===//

#ifndef TILELANG_DIALECT_TILELANG_TRANSFORMS_NPUIRTOTLIRCONVERSION_H
#define TILELANG_DIALECT_TILELANG_TRANSFORMS_NPUIRTOTLIRCONVERSION_H

#include <memory>

namespace mlir {
class Pass;

namespace tilelang {

/// Create a pass to convert NPUIR (HIVM) dialect operations to TLIR (Tilelang)
/// dialect operations.
std::unique_ptr<Pass> createNpuirToTlirConversionPass();

} // namespace tilelang
} // namespace mlir

#endif // TILELANG_DIALECT_TILELANG_TRANSFORMS_NPUIRTOTLIRCONVERSION_H
