//===- NpuirToTlirConversion.cpp - NPUIR to TLIR Conversion Pass ---------===//
//
// Copyright (c) Tile-AI Corporation.
// Licensed under the MIT License.
//
//===----------------------------------------------------------------------===//
//
// This file implements the NPUIR (HIVM) to TLIR (Tilelang) conversion pass.
//
//===----------------------------------------------------------------------===//

#include "tilelang/Dialect/Tilelang/Transforms/NpuirToTlirConversion.h"

#include "bishengir/Dialect/HIVM/IR/HIVM.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "llvm/ADT/SmallVector.h"

namespace mlir {
namespace tilelang {

namespace {

//===----------------------------------------------------------------------===//
// Type Converter
//===----------------------------------------------------------------------===//

class NpuirToTlirTypeConverter : public TypeConverter {
public:
  NpuirToTlirTypeConverter() {
    // Add type conversions here
    // For now, we'll pass through most types unchanged
    addConversion([](Type type) { return type; });
  }
};

//===----------------------------------------------------------------------===//
// Conversion Patterns
//===----------------------------------------------------------------------===//

// Base pattern for HIVM to TLIR conversion
template <typename SourceOp>
class HivmOpConversionPattern : public OpConversionPattern<SourceOp> {
public:
  using OpConversionPattern<SourceOp>::OpConversionPattern;
};

// Example: Convert HIVM GetBlockIdxOp to TLIR equivalent
// This is a placeholder - actual implementation depends on TLIR having equivalent ops
class ConvertGetBlockIdxOp : public HivmOpConversionPattern<hivm::GetBlockIdxOp> {
public:
  using HivmOpConversionPattern::HivmOpConversionPattern;

  LogicalResult
  matchAndRewrite(hivm::GetBlockIdxOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // For now, we keep HIVM ops that don't have direct TLIR equivalents
    // In a full implementation, you would convert to appropriate TLIR ops
    // Example: rewriter.replaceOpWithNewOp<tilelang::GetProgramIdOp>(op, ...);
    return failure(); // Allow partial conversion to keep this op
  }
};

// Example: Convert HIVM LoadScalarOp to memref.load
class ConvertLoadScalarOp : public HivmOpConversionPattern<hivm::LoadScalarOp> {
public:
  using HivmOpConversionPattern::HivmOpConversionPattern;

  LogicalResult
  matchAndRewrite(hivm::LoadScalarOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Convert HIVM load to memref load
    // This is a simplified example - actual conversion may need type conversion
    auto loc = op.getLoc();

    // Get the pointer operand
    auto addr = adaptor.getAddr();

    // For LLVM pointer types, we might need to convert to memref first
    // This is a placeholder - actual implementation depends on your type system

    // For now, keep the operation as-is (partial conversion)
    return failure();
  }
};

//===----------------------------------------------------------------------===//
// Conversion Pass
//===----------------------------------------------------------------------===//

struct NpuirToTlirConversionPass
    : public PassWrapper<NpuirToTlirConversionPass, OperationPass<ModuleOp>> {

  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(NpuirToTlirConversionPass)

  StringRef getArgument() const final { return "npuir-to-tlir"; }

  StringRef getDescription() const final {
    return "Convert NPUIR (HIVM) dialect operations to TLIR (Tilelang) dialect"; 
  }

  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<arith::ArithDialect>();
    registry.insert<scf::SCFDialect>();
    registry.insert<math::MathDialect>();
    registry.insert<memref::MemRefDialect>();
    registry.insert<func::FuncDialect>();
  }

  void runOnOperation() override {
    auto module = getOperation();

    // Set up type converter
    NpuirToTlirTypeConverter typeConverter;

    // Set up conversion target
    ConversionTarget target(getContext());

    // Mark TLIR dialect and standard dialects as legal
    target.addLegalDialect<arith::ArithDialect>();
    target.addLegalDialect<scf::SCFDialect>();
    target.addLegalDialect<math::MathDialect>();
    target.addLegalDialect<memref::MemRefDialect>();
    target.addLegalDialect<func::FuncDialect>();

    // Mark HIVM dialect as illegal (to be converted)
    target.addIllegalDialect<hivm::HIVMDialect>();

    // However, allow HIVM ops that we can't convert yet (partial conversion)
    target.markUnknownOpDynamicallyLegal([](Operation *) { return true; });

    // Set up rewrite patterns
    RewritePatternSet patterns(&getContext());

    // Add conversion patterns
    // These are example patterns - you can add more as needed
    patterns.add<ConvertGetBlockIdxOp>(typeConverter, &getContext());
    patterns.add<ConvertLoadScalarOp>(typeConverter, &getContext());

    // TODO: Add more conversion patterns here as you identify HIVM ops to convert
    // Example patterns to add:
    // patterns.add<ConvertHivmDotOp>(typeConverter, &getContext());
    // patterns.add<ConvertHivmStoreOp>(typeConverter, &getContext());
    // patterns.add<ConvertHivmBroadcastOp>(typeConverter, &getContext());

    // Apply partial conversion
    if (failed(applyPartialConversion(module, target, std::move(patterns)))) {
      signalPassFailure();
    }
  }
};

} // namespace

//===----------------------------------------------------------------------===//
// Pass Creation
//===----------------------------------------------------------------------===//

std::unique_ptr<Pass> createNpuirToTlirConversionPass() {
  return std::make_unique<NpuirToTlirConversionPass>();
}

} // namespace tilelang
} // namespace mlir
