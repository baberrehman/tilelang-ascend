# NPUIR to TLIR Conversion Pass - Implementation Summary

## Overview

This document summarizes the implementation of the NPUIR (HIVM dialect) to TLIR (Tilelang dialect) conversion pass for the TileLang project.

## Files Created

### 1. Pass Definition
**File**: `include/tilelang/Dialect/Tilelang/Transforms/Passes.td`
- Added `NpuirToTlirConversion` pass definition
- Configured to operate on `mlir::ModuleOp`
- Declared dependencies on required dialects (Tilelang, Arith, SCF, Math, MemRef)

### 2. Header File
**File**: `include/tilelang/Dialect/Tilelang/Transforms/NpuirToTlirConversion.h`
- Declares the pass factory function `createNpuirToTlirConversionPass()`
- Provides the public interface for the pass

### 3. Implementation File
**File**: `src/Dialect/Tilelang/Transforms/NpuirToTlirConversion.cpp`
- Implements the conversion pass class `NpuirToTlirConversionPass`
- Includes type converter `NpuirToTlirTypeConverter`
- Contains example conversion patterns:
  - `ConvertGetBlockIdxOp`: Converts HIVM block index operations
  - `ConvertLoadScalarOp`: Converts HIVM scalar load operations
- Uses partial conversion strategy (allows unconverted ops to remain)

### 4. Build Configuration
**Files Modified**:
- `CMakeLists.txt`: Added new source file to NPUIR source list
- `src/Dialect/Tilelang/Transforms/CMakeLists.txt`: Created build configuration for transforms
- `src/Dialect/Tilelang/CMakeLists.txt`: Created to include Transforms subdirectory
- `src/Dialect/CMakeLists.txt`: Created to include Tilelang subdirectory

### 5. Test File
**File**: `testing/npuir/npuir_to_tlir_basic.mlir`
- Basic test demonstrating pass can be loaded and run
- Can be extended with actual conversion tests

### 6. Documentation
**File**: `src/Dialect/Tilelang/Transforms/README.md`
- Comprehensive documentation of the pass
- Usage instructions
- Architecture overview
- Guide for adding new conversion patterns
- Key operation mappings

## Architecture

### Conversion Strategy
The pass uses **partial conversion**, which means:
- Operations with defined conversion patterns are converted to TLIR
- Operations without patterns remain as HIVM operations
- This allows gradual implementation of conversion patterns

### Type Conversion
- `NpuirToTlirTypeConverter` handles type conversions
- Currently passes most types through unchanged
- Can be extended to handle HIVM-specific types

### Conversion Patterns
The pass uses MLIR's `ConversionPattern` framework:
- Each HIVM operation type has a corresponding pattern class
- Patterns inherit from `HivmOpConversionPattern<SourceOp>`
- Patterns implement `matchAndRewrite` to perform the conversion

### Conversion Target
The pass configures:
- **Legal dialects** (target): Arith, SCF, Math, MemRef, Func
- **Illegal dialect** (to convert): HIVM
- **Dynamic legality**: Allows unconverted ops (partial conversion)

## Key Design Decisions

1. **Partial Conversion**: Chosen to allow incremental implementation
2. **Modular Patterns**: Each operation type has its own conversion pattern
3. **Standard Dialects**: Converts to standard MLIR dialects where possible
4. **Extensible**: Easy to add new conversion patterns

## Integration

The pass is integrated into the TileLang build system:
- Only built when `USE_NPUIR` is enabled
- Automatically included in the NPUIR source list
- Generates pass registration code via TableGen

## Usage

```bash
# Run the pass on an MLIR file
mlir-opt --npuir-to-tlir input.mlir -o output.mlir

# Chain with other passes
mlir-opt --npuir-to-tlir --canonicalize input.mlir
```

## Future Work

### Short Term
1. Implement conversion patterns for common HIVM operations:
   - Arithmetic operations (add, mul, sub, div)
   - Memory operations (load, store)
   - Control flow operations (for, if)
   - Cast operations

2. Add comprehensive tests for each conversion pattern

3. Implement type conversion for HIVM-specific types

### Medium Term
1. Add attribute conversion where needed
2. Optimize conversion patterns for performance
3. Add verification to ensure semantic correctness
4. Document specific operation mappings

### Long Term
1. Consider full conversion mode for specific use cases
2. Add optimization passes specific to converted code
3. Integrate with TileLang's compilation pipeline
4. Add benchmarks to measure conversion overhead

## Testing Strategy

1. **Unit Tests**: Test individual conversion patterns
2. **Integration Tests**: Test full conversion of HIVM modules
3. **Regression Tests**: Ensure conversions preserve semantics
4. **Performance Tests**: Measure conversion overhead

## Dependencies

- MLIR core libraries
- Arith dialect
- SCF dialect
- Math dialect
- MemRef dialect
- Func dialect
- HIVM dialect (from AscendNPU-IR)
- Tilelang dialect

## Build Verification

The implementation has been verified to:
- Be detected by CMake configuration
- Be included in the source file list
- Generate proper build files

Compilation verification is in progress.

## Notes

- The pass is designed to be extensible - new patterns can be added easily
- The partial conversion strategy allows for gradual implementation
- The pass follows MLIR best practices for dialect conversion
- Documentation is comprehensive to help future contributors
