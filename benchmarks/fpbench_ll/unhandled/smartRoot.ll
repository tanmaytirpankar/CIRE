; ModuleID = 'benchmarks_c/smartRoot.c'
source_filename = "benchmarks_c/smartRoot.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree nounwind willreturn memory(write) uwtable
define dso_local double @ex0(double noundef %c) local_unnamed_addr #0 {
entry:
  %mul1 = fmul double %c, 3.000000e+00
  %neg = fmul double %mul1, 4.000000e+00
  %0 = fsub double 1.225000e+01, %neg
  %1 = fsub double 1.225000e+01, %mul1
  %cmp = fcmp ogt double %1, 1.000000e+01
  %call = tail call double @sqrt(double noundef %0) #2, !tbaa !5
  br i1 %cmp, label %if.then6, label %if.else20

if.then6:                                         ; preds = %entry
  %mul = fmul double %c, 2.000000e+00
  %sub = fsub double -3.500000e+00, %call
  %div = fdiv double %mul, %sub
  br label %if.end26

if.else20:                                        ; preds = %entry
  %add23 = fadd double %call, -3.500000e+00
  %div25 = fdiv double %add23, 6.000000e+00
  br label %if.end26

if.end26:                                         ; preds = %if.else20, %if.then6
  %tmp_1.0 = phi double [ %div, %if.then6 ], [ %div25, %if.else20 ]
  ret double %tmp_1.0
}

; Function Attrs: mustprogress nofree nounwind willreturn memory(write)
declare double @sqrt(double noundef) local_unnamed_addr #1

attributes #0 = { mustprogress nofree nounwind willreturn memory(write) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree nounwind willreturn memory(write) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 20.0.0git (https://github.com/llvm/llvm-project.git 8ac924744e93258d0c490e2fa2d4518e24cb458d)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}