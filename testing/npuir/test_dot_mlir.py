import os

import tilelang
import tilelang.language as T

os.environ["TILELANG_ASCEND_MODE"] = "MLIR"
tilelang.cache.clear_cache()

M = 16
N = 32
K = 32
BLOCK_M = 16
BLOCK_N = 16
K_L1 = 16


def dot_kernel(
    M,
    N,
    K,
    block_M,
    block_N,
    k_l1,
    dtype="float16",
    accum_dtype="float32",
    a_transpose=False,
    b_transpose=False,
):
    m_num = M // block_M
    n_num = N // block_N

    @T.prim_func
    def main(
        A: T.Tensor((M, K), dtype),
        B: T.Tensor((K, N), dtype),
        C: T.Tensor((M, N), dtype),
    ):
        with T.Kernel(m_num * n_num, is_npu=True) as (cid, _):
            with T.Scope("Cube"):
                bx = cid // n_num * block_M
                by = cid % n_num * block_N

                A_BUF = T.alloc_L1((block_M, k_l1), dtype)
                B_BUF = T.alloc_L1((k_l1, block_N), dtype)
                C_BUF = T.alloc_L0C((block_M, block_N), accum_dtype)

                T.npuir_load_nd2nz(A[bx, 0], A_BUF, [block_M, k_l1])
                T.npuir_load_nd2nz(B[0, by], B_BUF, [k_l1, block_N])

                T.npuir_dot(
                    A_BUF,
                    B_BUF,
                    C_BUF,
                    [block_M, k_l1, block_N],
                    initC=True,
                    a_transpose=a_transpose,
                    b_transpose=b_transpose,
                )

                T.npuir_store_fixpipe(
                    C_BUF,
                    C[bx, by],
                    [block_M, block_N],
                    enable_nz2nd=True,
                )

    return main


def lower_to_mlir(func):
    lowered = tilelang.engine.lower(func, target="npuir")
    return str(lowered)


def test_dot_matmul_mlir():
    func = dot_kernel(M, N, K, BLOCK_M, BLOCK_N, K_L1)
    mlir = lower_to_mlir(func)
    assert "linalg.matmul" in mlir


def test_dot_matmul_transpose_a_mlir():
    func = dot_kernel(
        M,
        N,
        K,
        BLOCK_M,
        BLOCK_N,
        K_L1,
        a_transpose=True,
        b_transpose=False,
    )
    mlir = lower_to_mlir(func)
    assert "linalg.matmul_transpose_a" in mlir


def test_dot_matmul_transpose_b_mlir():
    func = dot_kernel(
        M,
        N,
        K,
        BLOCK_M,
        BLOCK_N,
        K_L1,
        a_transpose=False,
        b_transpose=True,
    )
    mlir = lower_to_mlir(func)
    assert "linalg.matmul_transpose_b" in mlir


if __name__ == "__main__":
    print("Running DotCodegen MLIR tests")
    test_dot_matmul_mlir()
    test_dot_matmul_transpose_a_mlir()
    test_dot_matmul_transpose_b_mlir()
    print("DotCodegen MLIR tests completed")
