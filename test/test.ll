; ModuleID = 'main'
source_filename = "main"

declare i32 @putchar(i32)

define i32 @main() {
entry:
  %a = alloca double
  %x = alloca double
  %y = alloca double
  store double 0.000000e+00, double* %y
  store double 1.500000e+00, double* %y
  %y1 = load double, double* %y
  %0 = fcmp ogt double %y1, -1.500000e+00
  %1 = uitofp i1 %0 to double
  %2 = fcmp one double %1, 0.000000e+00
  br i1 %2, label %for_loop, label %for_after22

for_loop:                                         ; preds = %for_after, %entry
  store double 0.000000e+00, double* %x
  store double -1.500000e+00, double* %x
  %x3 = load double, double* %x
  %3 = fcmp ult double %x3, 1.500000e+00
  %4 = uitofp i1 %3 to double
  %5 = fcmp one double %4, 0.000000e+00
  br i1 %5, label %for_loop2, label %for_after

for_loop2:                                        ; preds = %if_after, %for_loop
  store double 0.000000e+00, double* %a
  %x4 = load double, double* %x
  %x5 = load double, double* %x
  %6 = fmul double %x4, %x5
  %y6 = load double, double* %y
  %y7 = load double, double* %y
  %7 = fmul double %y6, %y7
  %8 = fadd double %6, %7
  %9 = fsub double %8, 1.000000e+00
  store double %9, double* %a
  %a8 = load double, double* %a
  %a9 = load double, double* %a
  %10 = fmul double %a8, %a9
  %a10 = load double, double* %a
  %11 = fmul double %10, %a10
  %x11 = load double, double* %x
  %x12 = load double, double* %x
  %12 = fmul double %x11, %x12
  %y13 = load double, double* %y
  %13 = fmul double %12, %y13
  %y14 = load double, double* %y
  %14 = fmul double %13, %y14
  %y15 = load double, double* %y
  %15 = fmul double %14, %y15
  %16 = fsub double %11, %15
  %17 = fcmp ole double %16, 0.000000e+00
  %18 = uitofp i1 %17 to double
  %19 = fcmp one double %18, 0.000000e+00
  br i1 %19, label %if_then, label %if_else

if_then:                                          ; preds = %for_loop2
  %cast = sext i8 42 to i32
  %20 = call i32 @putchar(i32 %cast)
  br label %if_after

if_else:                                          ; preds = %for_loop2
  %cast16 = sext i8 32 to i32
  %21 = call i32 @putchar(i32 %cast16)
  br label %if_after

if_after:                                         ; preds = %if_else, %if_then
  %x17 = load double, double* %x
  %22 = fadd double %x17, 5.000000e-02
  store double %22, double* %x
  %x18 = load double, double* %x
  %23 = fcmp ult double %x18, 1.500000e+00
  %24 = uitofp i1 %23 to double
  %25 = fcmp one double %24, 0.000000e+00
  br i1 %25, label %for_loop2, label %for_after

for_after:                                        ; preds = %if_after, %for_loop
  %cast19 = sext i8 10 to i32
  %26 = call i32 @putchar(i32 %cast19)
  %y20 = load double, double* %y
  %27 = fsub double %y20, 1.000000e-01
  store double %27, double* %y
  %y21 = load double, double* %y
  %28 = fcmp ogt double %y21, -1.500000e+00
  %29 = uitofp i1 %28 to double
  %30 = fcmp one double %29, 0.000000e+00
  br i1 %30, label %for_loop, label %for_after22

for_after22:                                      ; preds = %for_after, %entry
  ret i32 0
}
