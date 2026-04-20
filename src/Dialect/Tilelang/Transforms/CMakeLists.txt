add_mlir_dialect_library(TilelangTransforms
  NpuirToTlirConversion.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/tilelang/Dialect/Tilelang/Transforms

  DEPENDS
  TilelangTransformsIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRPass
  MLIRTransforms
  MLIRDialectUtils
  MLIRArithDialect
  MLIRSCFDialect
  MLIRMathDialect
  MLIRMemRefDialect
  MLIRFuncDialect
)
