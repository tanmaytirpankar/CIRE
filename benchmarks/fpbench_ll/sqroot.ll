; ModuleID = 'benchmarks_c/sqroot.c'
source_filename = "benchmarks_c/sqroot.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef double @ex0(double noundef %x) local_unnamed_addr #0 {
entry:
  %0 = tail call double @llvm.fmuladd.f64(double %x, double 5.000000e-01, double 1.000000e+00)
  %neg = fmul double %x, -1.250000e-01
  %1 = tail call double @llvm.fmuladd.f64(double %neg, double %x, double %0)
  %mul2 = fmul double %x, 6.250000e-02
  %mul3 = fmul double %mul2, %x
  %2 = tail call double @llvm.fmuladd.f64(double %mul3, double %x, double %1)
  %3 = fmul double %x, -3.906250e-02
  %4 = fmul double %3, %x
  %neg9 = fmul double %4, %x
  %5 = tail call double @llvm.fmuladd.f64(double %neg9, double %x, double %2)
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