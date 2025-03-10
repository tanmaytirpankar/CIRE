; ModuleID = '/home/tanmay/Documents/Tools/CIRE/benchmarks/fpbench_c/delta.c'
source_filename = "/home/tanmay/Documents/Tools/CIRE/benchmarks/fpbench_c/delta.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local double @ex0(double noundef %x1, double noundef %x2, double noundef %x3, double noundef %x4, double noundef %x5, double noundef %x6) local_unnamed_addr #0 {
entry:
  %mul = fmul double %x1, %x4
  %fneg = fneg double %x1
  %add = fsub double %x2, %x1
  %add1 = fadd double %add, %x3
  %sub = fsub double %add1, %x4
  %add2 = fadd double %sub, %x5
  %add3 = fadd double %add2, %x6
  %mul5 = fmul double %x2, %x5
  %sub6 = fsub double %x1, %x2
  %add7 = fadd double %sub6, %x3
  %add8 = fadd double %add7, %x4
  %sub9 = fsub double %add8, %x5
  %add10 = fadd double %sub9, %x6
  %mul11 = fmul double %mul5, %add10
  %0 = tail call double @llvm.fmuladd.f64(double %mul, double %add3, double %mul11)
  %mul12 = fmul double %x3, %x6
  %add13 = fadd double %x1, %x2
  %sub14 = fsub double %add13, %x3
  %add15 = fadd double %sub14, %x4
  %add16 = fadd double %add15, %x5
  %sub17 = fsub double %add16, %x6
  %1 = tail call double @llvm.fmuladd.f64(double %mul12, double %sub17, double %0)
  %fneg19 = fneg double %x2
  %mul20 = fmul double %fneg19, %x3
  %2 = tail call double @llvm.fmuladd.f64(double %mul20, double %x4, double %1)
  %mul23 = fmul double %fneg, %x3
  %3 = tail call double @llvm.fmuladd.f64(double %mul23, double %x5, double %2)
  %mul26 = fmul double %fneg, %x2
  %4 = tail call double @llvm.fmuladd.f64(double %mul26, double %x6, double %3)
  %fneg28 = fneg double %x4
  %mul29 = fmul double %fneg28, %x5
  %5 = tail call double @llvm.fmuladd.f64(double %mul29, double %x6, double %4)
  ret double %5
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #1

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 20.0.0git (https://github.com/llvm/llvm-project.git 8ac924744e93258d0c490e2fa2d4518e24cb458d)"}
