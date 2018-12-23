; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [17 x i8] c"Hello World! %d\0A\00"
@1 = private unnamed_addr constant [6 x i8] c"%.2f\0A\00"

declare i32 @putchar(i32)

define i32 @fuck(i32 %i) {
entry:
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %i2 = load i32, i32* %i1
  ret i32 %i2
}

define double @shit(double %i) {
entry:
  %i1 = alloca double
  store double %i, double* %i1
  %i2 = load double, double* %i1
  ret double %i2
}

define i32 @main() {
entry:
  %a37 = alloca double
  %i = alloca i32
  %a = alloca double
  %x = alloca double
  %y = alloca double
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @0, i32 0, i32 0), i32 42)
  store double 0.000000e+00, double* %y
  store double 1.500000e+00, double* %y
  %y1 = load double, double* %y
  %1 = fcmp ogt double %y1, -1.500000e+00
  %2 = uitofp i1 %1 to double
  %cond = fcmp one double %2, 0.000000e+00
  br i1 %cond, label %for_loop, label %for_after26

for_loop:                                         ; preds = %for_after, %entry
  store double 0.000000e+00, double* %x
  store double -1.500000e+00, double* %x
  %x3 = load double, double* %x
  %3 = fcmp ult double %x3, 1.500000e+00
  %4 = uitofp i1 %3 to double
  %cond4 = fcmp one double %4, 0.000000e+00
  br i1 %cond4, label %for_loop2, label %for_after

for_loop2:                                        ; preds = %if_after, %for_loop
  store double 0.000000e+00, double* %a
  %x5 = load double, double* %x
  %x6 = load double, double* %x
  %5 = fmul double %x5, %x6
  %y7 = load double, double* %y
  %y8 = load double, double* %y
  %6 = fmul double %y7, %y8
  %7 = fadd double %5, %6
  %8 = fsub double %7, 1.000000e+00
  store double %8, double* %a
  %a9 = load double, double* %a
  %a10 = load double, double* %a
  %9 = fmul double %a9, %a10
  %a11 = load double, double* %a
  %10 = fmul double %9, %a11
  %x12 = load double, double* %x
  %x13 = load double, double* %x
  %11 = fmul double %x12, %x13
  %y14 = load double, double* %y
  %12 = fmul double %11, %y14
  %y15 = load double, double* %y
  %13 = fmul double %12, %y15
  %y16 = load double, double* %y
  %14 = fmul double %13, %y16
  %15 = fsub double %10, %14
  %16 = fcmp ole double %15, 0.000000e+00
  %17 = uitofp i1 %16 to double
  %cond17 = fcmp one double %17, 0.000000e+00
  br i1 %cond17, label %if_then, label %if_else

if_then:                                          ; preds = %for_loop2
  %cast = sext i8 42 to i32
  %18 = call i32 @putchar(i32 %cast)
  br label %if_after

if_else:                                          ; preds = %for_loop2
  %cast18 = sext i8 32 to i32
  %19 = call i32 @putchar(i32 %cast18)
  br label %if_after

if_after:                                         ; preds = %if_else, %if_then
  %x19 = load double, double* %x
  %20 = fadd double %x19, 5.000000e-02
  store double %20, double* %x
  %x20 = load double, double* %x
  %21 = fcmp ult double %x20, 1.500000e+00
  %22 = uitofp i1 %21 to double
  %cond21 = fcmp one double %22, 0.000000e+00
  br i1 %cond21, label %for_loop2, label %for_after

for_after:                                        ; preds = %if_after, %for_loop
  %cast22 = sext i8 10 to i32
  %23 = call i32 @putchar(i32 %cast22)
  %y23 = load double, double* %y
  %24 = fsub double %y23, 1.000000e-01
  store double %24, double* %y
  %y24 = load double, double* %y
  %25 = fcmp ogt double %y24, -1.500000e+00
  %26 = uitofp i1 %25 to double
  %cond25 = fcmp one double %26, 0.000000e+00
  br i1 %cond25, label %for_loop, label %for_after26

for_after26:                                      ; preds = %for_after, %entry
  %27 = call i32 @fuck(i32 0)
  %28 = icmp ne i32 %27, 0
  %29 = icmp ne i1 %28, true
  br i1 %29, label %if_then27, label %if_after28

if_then27:                                        ; preds = %for_after26
  %30 = call i32 @putchar(i32 48)
  br label %if_after28

if_after28:                                       ; preds = %if_then27, %for_after26
  %31 = call i32 @fuck(i32 2)
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %if_then29, label %if_after30

if_then29:                                        ; preds = %if_after28
  %33 = call i32 @putchar(i32 49)
  br label %if_after30

if_after30:                                       ; preds = %if_then29, %if_after28
  %34 = call i32 @fuck(i32 2)
  %35 = xor i32 %34, -1
  %36 = icmp ne i32 %35, 0
  br i1 %36, label %if_then31, label %if_after32

if_then31:                                        ; preds = %if_after30
  %37 = call i32 @putchar(i32 50)
  br label %if_after32

if_after32:                                       ; preds = %if_then31, %if_after30
  %38 = call i32 @fuck(i32 48)
  %39 = call i32 @putchar(i32 %38)
  %40 = call i32 @putchar(i32 -3)
  %41 = call i32 @putchar(i32 2)
  %42 = call i32 @putchar(i32 0)
  %43 = call i32 @putchar(i32 0)
  %44 = call i32 @putchar(i32 10)
  store i32 0, i32* %i
  store i32 9, i32* %i
  %i33 = load i32, i32* %i
  %45 = icmp sgt i32 %i33, 0
  br i1 %45, label %while_loop, label %while_after

while_loop:                                       ; preds = %while_loop, %if_after32
  %i34 = load i32, i32* %i
  %46 = add i32 %i34, 48
  %47 = call i32 @putchar(i32 %46)
  %i35 = load i32, i32* %i
  %48 = sub i32 %i35, 1
  store i32 %48, i32* %i
  %i36 = load i32, i32* %i
  %49 = icmp sgt i32 %i36, 0
  br i1 %49, label %while_loop, label %while_after

while_after:                                      ; preds = %while_loop, %if_after32
  store double 0.000000e+00, double* %a37
  %cast38 = sitofp i32 1 to double
  store double %cast38, double* %a37
  %a39 = load double, double* %a37
  %50 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @1, i32 0, i32 0), double %a39)
  %cast40 = fptosi double 4.860000e+01 to i32
  %51 = call i32 @putchar(i32 %cast40)
  ret i32 0
}

declare i32 @printf(i8*, ...)
