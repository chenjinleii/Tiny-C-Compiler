; ModuleID = 'main'
source_filename = "main"

declare i32 @putchar(i32)

define i32 @fuck(i32 %i) {
entry:
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %i2 = load i32, i32* %i1
  ret i32 %i2
}

define i32 @main() {
entry:
  %i = alloca i32
  %a = alloca double
  %x = alloca double
  %y = alloca double
  store double 0.000000e+00, double* %y
  store double 1.500000e+00, double* %y
  %y1 = load double, double* %y
  %0 = fcmp ogt double %y1, -1.500000e+00
  %1 = uitofp i1 %0 to double
  %cond = fcmp one double %1, 0.000000e+00
  br i1 %cond, label %for_loop, label %for_after26

for_loop:                                         ; preds = %for_after, %entry
  store double 0.000000e+00, double* %x
  store double -1.500000e+00, double* %x
  %x3 = load double, double* %x
  %2 = fcmp ult double %x3, 1.500000e+00
  %3 = uitofp i1 %2 to double
  %cond4 = fcmp one double %3, 0.000000e+00
  br i1 %cond4, label %for_loop2, label %for_after

for_loop2:                                        ; preds = %if_after, %for_loop
  store double 0.000000e+00, double* %a
  %x5 = load double, double* %x
  %x6 = load double, double* %x
  %4 = fmul double %x5, %x6
  %y7 = load double, double* %y
  %y8 = load double, double* %y
  %5 = fmul double %y7, %y8
  %6 = fadd double %4, %5
  %7 = fsub double %6, 1.000000e+00
  store double %7, double* %a
  %a9 = load double, double* %a
  %a10 = load double, double* %a
  %8 = fmul double %a9, %a10
  %a11 = load double, double* %a
  %9 = fmul double %8, %a11
  %x12 = load double, double* %x
  %x13 = load double, double* %x
  %10 = fmul double %x12, %x13
  %y14 = load double, double* %y
  %11 = fmul double %10, %y14
  %y15 = load double, double* %y
  %12 = fmul double %11, %y15
  %y16 = load double, double* %y
  %13 = fmul double %12, %y16
  %14 = fsub double %9, %13
  %15 = fcmp ole double %14, 0.000000e+00
  %16 = uitofp i1 %15 to double
  %cond17 = fcmp one double %16, 0.000000e+00
  br i1 %cond17, label %if_then, label %if_else

if_then:                                          ; preds = %for_loop2
  %cast = sext i8 42 to i32
  %17 = call i32 @putchar(i32 %cast)
  br label %if_after

if_else:                                          ; preds = %for_loop2
  %cast18 = sext i8 32 to i32
  %18 = call i32 @putchar(i32 %cast18)
  br label %if_after

if_after:                                         ; preds = %if_else, %if_then
  %x19 = load double, double* %x
  %19 = fadd double %x19, 5.000000e-02
  store double %19, double* %x
  %x20 = load double, double* %x
  %20 = fcmp ult double %x20, 1.500000e+00
  %21 = uitofp i1 %20 to double
  %cond21 = fcmp one double %21, 0.000000e+00
  br i1 %cond21, label %for_loop2, label %for_after

for_after:                                        ; preds = %if_after, %for_loop
  %cast22 = sext i8 10 to i32
  %22 = call i32 @putchar(i32 %cast22)
  %y23 = load double, double* %y
  %23 = fsub double %y23, 1.000000e-01
  store double %23, double* %y
  %y24 = load double, double* %y
  %24 = fcmp ogt double %y24, -1.500000e+00
  %25 = uitofp i1 %24 to double
  %cond25 = fcmp one double %25, 0.000000e+00
  br i1 %cond25, label %for_loop, label %for_after26

for_after26:                                      ; preds = %for_after, %entry
  %26 = call i32 @fuck(i32 0)
  %27 = icmp ne i32 %26, 0
  %28 = icmp ne i1 %27, true
  br i1 %28, label %if_then27, label %if_after28

if_then27:                                        ; preds = %for_after26
  %29 = call i32 @putchar(i32 48)
  br label %if_after28

if_after28:                                       ; preds = %if_then27, %for_after26
  %30 = call i32 @fuck(i32 2)
  %31 = icmp ne i32 %30, 0
  br i1 %31, label %if_then29, label %if_after30

if_then29:                                        ; preds = %if_after28
  %32 = call i32 @putchar(i32 49)
  br label %if_after30

if_after30:                                       ; preds = %if_then29, %if_after28
  %33 = call i32 @fuck(i32 2)
  %34 = xor i32 %33, -1
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %if_then31, label %if_after32

if_then31:                                        ; preds = %if_after30
  %36 = call i32 @putchar(i32 50)
  br label %if_after32

if_after32:                                       ; preds = %if_then31, %if_after30
  %37 = call i32 @fuck(i32 48)
  %38 = call i32 @putchar(i32 %37)
  %39 = call i32 @putchar(i32 -3)
  %40 = call i32 @putchar(i32 2)
  %41 = call i32 @putchar(i32 0)
  %42 = call i32 @putchar(i32 0)
  %43 = call i32 @putchar(i32 10)
  store i32 0, i32* %i
  store i32 9, i32* %i
  %i33 = load i32, i32* %i
  %44 = icmp sgt i32 %i33, 0
  br i1 %44, label %while_loop, label %while_after

while_loop:                                       ; preds = %while_loop, %if_after32
  %i34 = load i32, i32* %i
  %45 = add i32 %i34, 48
  %46 = call i32 @putchar(i32 %45)
  %i35 = load i32, i32* %i
  %47 = sub i32 %i35, 1
  store i32 %47, i32* %i
  %i36 = load i32, i32* %i
  %48 = icmp sgt i32 %i36, 0
  br i1 %48, label %while_loop, label %while_after

while_after:                                      ; preds = %while_loop, %if_after32
  ret i32 0
}
