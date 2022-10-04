#include <iostream>
#include <omp.h>
#include <chrono>

void z1()
{
#pragma omp parallel num_threads(8)
	printf("Hello World! from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
}

void z2() {
	int val1 = 3;
	int val2 = 2;

#pragma omp parallel if (val1 > 2) num_threads(val1)
	printf_s("1st area, val = %d, %d thread of %d threads\n",
		val1, omp_get_thread_num(), omp_get_num_threads());

#pragma omp parallel if (val2 > 2) num_threads(val2)
	printf_s("2nd area, val = %d, %d thread of %d threads\n",
		val2, omp_get_thread_num(), omp_get_num_threads());
}

void z3() {
	int a = 24;
	int b = 24;

	printf("before 1st area: a = %d  b = %d\n", a, b);
#pragma omp parallel num_threads(2) private(a) firstprivate(b)
	{
		int tNumber;
		tNumber = omp_get_thread_num();
        a = 0;


		a += tNumber;
		b += tNumber;
		printf("in 1st area: thread %d, a = %d b = %d\n", tNumber, a, b);
	}
	printf("after 1st area: a = %d  b = %d\n", a, b);

	printf("\n******************\n");

	printf("\nbefore 2nd area: a = %d  b = %d\n", a, b);
#pragma omp parallel num_threads(4) shared(a) private(b)
	{
		int tNumber;
		tNumber = omp_get_thread_num();
        b = 0;

		a -= tNumber;
		b -= tNumber;
		printf("in 2nd area: thread %d, a = %d b = %d\n", tNumber, a, b);
	}
	printf("after 2nd area: a = %d  b = %d\n", a, b);
}

void z4() {
    int a[10];
    int b[10];

    int min = 20;
    int max = -1;

    for (int i = 0; i < 10; i++) {
        a[i] = i * 2;
        b[i] = i * 2;
    }

#pragma omp parallel shared(a, b, min, max) num_threads(2)
    {
        if (omp_get_thread_num() == 0) {
            for (int i : a) {
                if (i < min)
                    min = i;
            }
        }
        else {
            for (int i : b) {
                if (i > max)
                    max = i;
            }
        }
    }
    printf("Min = %d, Max = %d", min, max);
}

void z5() {
    int a[6][8];
    srand(std::chrono::steady_clock::now().time_since_epoch().count());

    for (auto& i : a) {
        for (int& j : i) {
            j = rand() % 30 + 1;
            printf("%d ", j);
        }
        printf("\n");
    }
#pragma omp parallel sections shared(a)
    {
#pragma omp section
        {
            int sum = 0;

            for (auto& i : a) {
                for (int& j : i) {
                    sum += j;
                }
            }
            printf("1st section, thread - %d, answer: %d \n", omp_get_thread_num(), sum / 48);
        }
#pragma omp section
        {
            int min = 21;
            int max = -1;

            for (auto& i : a) {
                for (int& j : i) {
                    if (j < min)
                        min = j;
                    if (j > max)
                        max = j;
                }
            }
            printf("2nd section, thread - %d, answer: min = %d, max = %d  \n", omp_get_thread_num(), min, max);
        }
#pragma omp section
        {
            int count = 0;

            for (auto& i : a) {
                for (int& j : i) {
                    if (j % 3 == 0)
                        count++;
                }
            }
            printf("3th section, thread - %d, answer: %d  \n", omp_get_thread_num(), count);
        }
    }
}

void z6() {
    int a[100];
    int sum1 = 0, sum2 = 0;

    for (int i = 0; i < 100; i++)
        a[i] = i;

#pragma omp parallel for
    for (int i = 0; i < 100; i++)
        sum1 += a[i];

    printf("parallel for, answer: %d\n", sum1 / 100);

#pragma omp parallel for reduction(+:sum2)
    for (int i = 0; i < 100; i++)
        sum2 += a[i];

    printf("parallel for with reduction, answer: %d", sum2 / 100);
}

void z7() {
    int a[12], b[12], c[12];

#pragma omp parallel for num_threads(3) shared(a, b) schedule (static, 4)
    for (int i = 0; i < 12; i++) {
        a[i] = i * 2;
        b[i] = i * 4;
        printf("static area, %d thread of %d threads. a[%d] = %d, b[%d] = %d\n", omp_get_thread_num(),
            omp_get_num_threads(), i, a[i], i, b[i]);
    }

#pragma omp parallel for num_threads(4) shared(a, b, c) schedule (dynamic, 2)
    for (int i = 0; i < 12; i++) {
        c[i] = a[i] + b[i];
        printf("dynamic area, %d thread of %d threads. c[%d] = %d + %d = %d\n", omp_get_thread_num(),
            omp_get_num_threads(), i, a[i], b[i], c[i]);
    }
}

void z8() {
    double a[16000], b[16000];
    int scheduleSize = 5;

    for (int i = 0; i < 16000; i++)
        a[i] = i;

    double startTime = omp_get_wtime();
#pragma omp parallel for  num_threads(8) shared(a, b, scheduleSize) schedule(static, scheduleSize)
    for (int i = 1; i < 15999; i++)
        b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    double endTime = omp_get_wtime();

    printf("static schedule, runtime = %f\n", endTime - startTime);

    startTime = omp_get_wtime();
#pragma omp parallel for  num_threads(8) shared(a, b, scheduleSize) schedule(dynamic, scheduleSize)
    for (int i = 1; i < 15999; i++)
        b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;

    endTime = omp_get_wtime();
    printf("dynamic schedule, runtime = %f\n", endTime - startTime);

    startTime = omp_get_wtime();
#pragma omp parallel for  num_threads(8) shared(a, b, scheduleSize) schedule(guided, scheduleSize)
    for (int i = 1; i < 15999; i++)
        b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;

    endTime = omp_get_wtime();
    printf("guided schedule, runtime = %f\n", endTime - startTime);
}

const int n = 2000;
long matrix[n][n];
long vector[n];

void z9() {
    
    int parallelAnswer[n];
    int answer[n];
    int sum = 0;

    srand(std::chrono::steady_clock::now().time_since_epoch().count());

    for (int i = 0; i < n; i++) {
        vector[i] = rand() % 5000 + 500;
        for (int j = 0; j < n; j++)
            matrix[i][j] = rand() % 7500 + 500;
    }

    float startTimeForParallel = float(clock()) / CLOCKS_PER_SEC;

#pragma omp parallel for shared(n, matrix, vector, parallelAnswer) schedule(dynamic, 10) reduction(+:sum)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            sum += matrix[i][j] * vector[j];
        parallelAnswer[i] = sum;
        sum = 0;
    }

    float endTimeForParallel = float(clock()) / CLOCKS_PER_SEC;

    printf("parallel answer (%f sec) \n", endTimeForParallel - startTimeForParallel);

    float startTime = float(clock()) / CLOCKS_PER_SEC;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            answer[i] = matrix[i][j] * vector[j];
        answer[i] = sum;
        sum = 0;
    }

    float endTime = float(clock()) / CLOCKS_PER_SEC;
    printf("answer (%f sec): \n", endTime - startTime);
}

void z10() {
    const int N = 6;
    const int M = 8;

    int d[N][M];
    int max = INT_MIN;
    int min = INT_MAX;

    srand(std::chrono::steady_clock::now().time_since_epoch().count());

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            d[i][j] = rand();
           // printf("%d ", d[i][j]);
        }
    }

#pragma omp parallel for num_threads(4)
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (d[i][j] > max)
#pragma omp critical
            {
                if (d[i][j] > max)
                max = d[i][j];
            }

            if (d[i][j] < min)
#pragma omp critical
            {
                if (d[i][j] < min)
                min = d[i][j];
            }

        }
    }
    printf("Max: %d; Min: %d", max, min);
}

void z11() {
    const int N = 30;
    int a[N];

    srand(std::chrono::steady_clock::now().time_since_epoch().count());

    for (int i = 0; i < N; i++)
    {
        a[i] = rand() % 1000;
      //  printf("%d ", a[i]);
    }
    printf("\n");

    int res = 0;

#pragma omp parallel for num_threads(8)
    for (int i = 0; i < N; i++)
    {
        if (a[i] % 9 == 0)
        {
#pragma omp atomic
            res++;
        }
    }
    printf("Result: %d", res);
}

void z12(int N) {
    int* a = new int[N];

    srand(std::chrono::steady_clock::now().time_since_epoch().count());

    for (int i = 0; i < N; i++)
    {
        a[i] = rand();
    }

    int max = INT_MIN;

#pragma omp parallel for num_threads(8)
    for (int i = 0; i < N; i++)
    {
        if (a[i] % 7 == 0)
        {
#pragma omp critical
            {
                if (max < a[i])
                {
                    max = a[i];
                }
            }
        }
    }

    printf("Max: %d", max);
}


int main()
{
	//z1();
	//z2();
	//z3();
    //z4();
    //z5();
    //z6();
    //z7();
    //z8();
    //z9();
    //z10();
    //z11();
    z12(10);
}