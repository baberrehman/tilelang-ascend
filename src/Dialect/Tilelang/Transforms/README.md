# NPUIR to TLIR Conversion Pass

This directory contains the implementation of the NPUIR (HIVM dialect) to TLIR (Tilelang dialect) conversion pass.

## Overview

The conversion pass transforms HIVM dialect operations into Tilelang (TLIR) dialect operations, enabling interoperability between the two dialects. The pass uses MLIR's dialect conversion framework and performs a **partial conversion**, meaning that operations without direct TLIR equivalents remain as HIVM operations.

## Files

- **NpuirToTlirConversion.h**: Header file declaring the pass factory function
- **NpuirToTlirConversion.cpp**: Implementation of the conversion pass and conversion patterns
- **CMakeLists.txt**: Build configuration for the pass

## Usage

The pass can be invoked using the `--npuir-to-tlir` flag with `mlir-opt`:

```bash
mlir-opt --npuir-to-tlir input.mlir -o output.mlir
```

## Architecture

### Type Converter
- `NpuirToTlirTypeConverter`: Handles type conversions between HIVM and TLIR types
- Currently passes most types through unchanged

### Conversion Patterns
The pass includes conversion patterns for various HIVM operations:

- `ConvertGetBlockIdxOp`: Converts HIVM block index operations
- `ConvertLoadScalarOp`: Converts HIVM scalar load operations
- More patterns can be added as needed

### Conversion Target
The pass marks the following dialects as legal (target):
- Arith dialect
- SCF (Structured Control Flow) dialect
- Math dialect
- MemRef dialect
- Func dialect

HIVM dialect operations are marked as illegal (to be converted), but the pass allows unconverted operations to remain (partial conversion).

## Adding New Conversion Patterns

To add a new conversion pattern:

1. Create a new pattern class inheriting from `HivmOpConversionPattern<SourceOp>`
2. Implement the `matchAndRewrite` method
3. Register the pattern in the `runOnOperation` method

Example:

```cpp
class ConvertMyHivmOp : public HivmOpConversionPattern<hivm::MyOp> {
public:
  using HivmOpConversionPattern::HivmOpConversionPattern;

  LogicalResult
  matchAndRewrite(hivm::MyOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Conversion logic here
    rewriter.replaceOpWithNewOp<tilelang::MyTlirOp>(op, ...);
    return success();
  }
};

// Register in runOnOperation():
patterns.add<ConvertMyHivmOp>(typeConverter, &getContext());
```

## Key Mappings

The following are the intended mappings from HIVM to TLIR operations:

### Arithmetic Operations
- HIVM arithmetic ops → Arith dialect ops (addf, muli, etc.)

### Memory Operations
- `hivm.load` → `tt.load`
- `hivm.store` → `tt.store`

### Control Flow
- HIVM control flow → SCF dialect ops (for, if, etc.)

### Cast Operations
- `hivm.bitcast` → `tt.bitcast`
- HIVM type conversions → Arith dialect conversions

### Compute Operations
- HIVM dot operations → `tt.dot` (if available)
- HIVM broadcast → `tt.broadcast`
- HIVM splat → `tt.splat`

## Testing

Test files are located in `testing/npuir/`:
- `npuir_to_tlir_basic.mlir`: Basic test demonstrating pass functionality

## Future Work

- Implement comprehensive conversion patterns for all HIVM operations
- Add type conversion logic for HIVM-specific types
- Implement attribute conversion where needed
- Add more comprehensive tests
- Document specific operation mappings as they are implemented

## Dependencies

The pass depends on:
- MLIR core libraries
- Arith dialect
- SCF dialect
- Math dialect
- MemRef dialect
- Func dialect
- HIVM dialect (from AscendNPU-IR)
- Tilelang dialect

## Build Configuration

The pass is built as part of the TileLang project when `USE_NPUIR` is enabled. It is automatically included in the build when NPUIR support is configured.
