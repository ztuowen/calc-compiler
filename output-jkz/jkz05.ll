; ModuleID = 'calc'
source_filename = "calc"
target triple = "x86_64-unknown-linux-gnu"

define i64 @f(i64, i64, i64, i64, i64, i64) {
entry:
  br label %6

; <label>:6:                                      ; preds = %29, %entry
  %7 = phi i64 [ 0, %entry ], [ %19, %29 ]
  %8 = phi i64 [ 0, %entry ], [ %23, %29 ]
  %9 = phi i64 [ 0, %entry ], [ %9, %29 ]
  %10 = phi i64 [ 0, %entry ], [ %10, %29 ]
  %11 = phi i64 [ 0, %entry ], [ %11, %29 ]
  %12 = phi i64 [ 0, %entry ], [ %12, %29 ]
  %13 = phi i64 [ 0, %entry ], [ %13, %29 ]
  %14 = phi i64 [ 0, %entry ], [ %14, %29 ]
  %15 = phi i64 [ 0, %entry ], [ %15, %29 ]
  %16 = phi i64 [ 0, %entry ], [ %16, %29 ]
  %17 = phi i64 [ 0, %entry ], [ %23, %29 ]
  %18 = call { i64, i1 } @llvm.sadd.with.overflow.i64(i64 %7, i64 1)
  %19 = extractvalue { i64, i1 } %18, 0
  %20 = extractvalue { i64, i1 } %18, 1
  br i1 %20, label %28, label %26

; <label>:21:                                     ; preds = %26
  %22 = call { i64, i1 } @llvm.sadd.with.overflow.i64(i64 %8, i64 1)
  %23 = extractvalue { i64, i1 } %22, 0
  %24 = extractvalue { i64, i1 } %22, 1
  br i1 %24, label %30, label %29

; <label>:25:                                     ; preds = %26
  ret i64 %19

; <label>:26:                                     ; preds = %6
  %27 = icmp slt i64 %8, 10
  br i1 %27, label %21, label %25

; <label>:28:                                     ; preds = %6
  call void @overflow_fail(i32 46)
  unreachable

; <label>:29:                                     ; preds = %21
  br label %6

; <label>:30:                                     ; preds = %21
  call void @overflow_fail(i32 123)
  unreachable
}

; Function Attrs: noreturn
declare void @overflow_fail(i32) #0

; Function Attrs: nounwind readnone
declare { i64, i1 } @llvm.sadd.with.overflow.i64(i64, i64) #1

; Function Attrs: nounwind readnone
declare { i64, i1 } @llvm.ssub.with.overflow.i64(i64, i64) #1

; Function Attrs: nounwind readnone
declare { i64, i1 } @llvm.smul.with.overflow.i64(i64, i64) #1

define i64 @_strap_div(i64, i64, i32) {
  %4 = icmp eq i64 %1, 0
  %5 = icmp eq i64 %0, -9223372036854775808
  %6 = icmp eq i64 %1, -1
  %7 = and i1 %5, %6
  %8 = or i1 %4, %7
  br i1 %8, label %11, label %9

; <label>:9:                                      ; preds = %3
  %10 = sdiv i64 %0, %1
  ret i64 %10

; <label>:11:                                     ; preds = %3
  call void @overflow_fail(i32 %2)
  unreachable
}

define i64 @_strap_mod(i64, i64, i32) {
  %4 = icmp eq i64 %1, 0
  br i1 %4, label %7, label %5

; <label>:5:                                      ; preds = %3
  %6 = srem i64 %0, %1
  ret i64 %6

; <label>:7:                                      ; preds = %3
  call void @overflow_fail(i32 %2)
  unreachable
}

attributes #0 = { noreturn }
attributes #1 = { nounwind readnone }
