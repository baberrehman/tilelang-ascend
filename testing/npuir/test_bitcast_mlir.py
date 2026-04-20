import torch
import tilelang
import tilelang.language as T
import os

os.environ["TILELANG_ASCEND_MODE"] = "MLIR"
tilelang.cache.clear_cache()

M = 16
N = 32
BLOCK_M = 16
BLOCK_N = 32


def bitcast_kernel(M, N, block_M, block_N, src_dtype="float16", dst_dtype="int16"):
    m_num = M // block_M
    n_num = N // block_N

    @T.prim_func
    def main(
            A: T.Tensor((M, N), src_dtype),
    ):
        with T.Kernel(m_num * n_num, is_npu=True) as (cid, _):
            bx_ = cid // n_num
            bx = bx_ * block_M
            by_ = cid % n_num
            by = by_ * block_N

            A_VEC = T.alloc_ub((block_M, block_N), src_dtype)
            T.copy(A[bx, by], A_VEC)
            T.npuir_bitcast(A_VEC, dst_dtype)

    return main


def test_bitcast_f16_to_i16():
    """Test bitcast from float16 to int16 (both 16-bit)."""
    func = bitcast_kernel(M, N, BLOCK_M, BLOCK_N, "float16", "int16")
    compiled_kernel = tilelang.engine.lower(func, target="npuir")
    return
    A = torch.randn((M, N), dtype=torch.float16).npu()
    Out = torch.zeros((M, N), dtype=torch.int16).npu()

    compiled_kernel(A, Out)

    ref = A.cpu().view(torch.int16)
    torch.testing.assert_close(Out.cpu(), ref)
    print("\033[92mBitcast float16 -> int16: PASS\033[0m")


def test_bitcast_f32_to_i32():
    """Test bitcast from float32 to int32 (both 32-bit)."""
    func = bitcast_kernel(M, N, BLOCK_M, BLOCK_N, "float32", "int32")
    compiled_kernel = tilelang.engine.lower(func, target="npuir")
    return
    A = torch.randn((M, N), dtype=torch.float32).npu()
    Out = torch.zeros((M, N), dtype=torch.int32).npu()

    compiled_kernel(A, Out)

    ref = A.cpu().view(torch.int32)
    torch.testing.assert_close(Out.cpu(), ref)
    print("\033[92mBitcast float32 -> int32: PASS\033[0m")


def test_bitcast_i16_to_f16():
    """Test bitcast from int16 to float16 (both 16-bit)."""
    func = bitcast_kernel(M, N, BLOCK_M, BLOCK_N, "int16", "float16")
    compiled_kernel = tilelang.engine.lower(func, target="npuir")
    return
    A = torch.randint(-32768, 32767, (M, N), dtype=torch.int16).npu()
    Out = torch.zeros((M, N), dtype=torch.float16).npu()

    compiled_kernel(A, Out)

    ref = A.cpu().view(torch.float16)
    torch.testing.assert_close(Out.cpu(), ref)
    print("\033[92mBitcast int16 -> float16: PASS\033[0m")


if __name__ == "__main__":
    print("Running bitcast MLIR codegen tests")
    test_bitcast_f16_to_i16()
    test_bitcast_f32_to_i32()
    test_bitcast_i16_to_f16()
    print("All bitcast tests completed.")
