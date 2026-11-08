// ==========================================================
//  نظام إدارة وتحليل نتائج الطلاب (Console-Based) - C++
//  المقابل في C++ لدوال المصفوفات:
//    reduce  -> std::accumulate
//    filter  -> std::copy_if
//    map     -> std::transform
// ==========================================================

#include <iostream>
#include <vector>
#include <string>
#include <numeric>    // accumulate
#include <algorithm>  // copy_if, transform
#include <iterator>   // back_inserter
#include <iomanip>    // setprecision

#ifdef _WIN32
#include <windows.h>  // لعرض الحروف العربية بشكل صحيح على ويندوز
#endif

using namespace std;

// ----------------------------------------------------------
// 1. هيكلة بيانات الطالب (Class & Constructor)
// ----------------------------------------------------------
class Student {
private:
    int id;                 // رقم الجلوس
    string name;            // الاسم
    vector<double> grades;  // درجات 3 مواد
    double attendance;      // نسبة الحضور

public:
    Student(int id, const string& name, const vector<double>& grades, double attendance)
        : id(id), name(name), grades(grades), attendance(attendance) {}

    int getId() const { return id; }
    string getName() const { return name; }
    vector<double> getGrades() const { return grades; }
    double getAttendance() const { return attendance; }

    // ------------------------------------------------------
    // 2. دوال التحليل الأكاديمي (Methods)
    // ------------------------------------------------------

    // دالة حساب المتوسط: reduce (accumulate) لجمع الدرجات ثم القسمة على عدد المواد
    double calculateAverage() const {
        if (grades.empty()) return 0.0;  // حماية من القسمة على صفر
        double total = accumulate(grades.begin(), grades.end(), 0.0,
                                  [](double sum, double grade) { return sum + grade; });
        return total / grades.size();
    }

    // دالة التقدير: تستقبل المتوسط وترجع التقدير بالحروف
    string getLetterGrade(double average) const {
        if (average >= 90) {
            return "A";
        } else if (average >= 80) {
            return "B";
        } else if (average >= 70) {
            return "C";
        } else if (average >= 60) {
            return "D";
        } else if (average >= 50) {
            return "E";
        } else {
            return "F";
        }
    }

    // دالة النتيجة النهائية: المتوسط >= 50 && الحضور >= 75
    string getFinalResult() const {
        double average = calculateAverage();
        if (average >= 50 && attendance >= 75) {
            return "ناجح";
        } else {
            return "يحتاج مراجعة";
        }
    }
};

// هيكل لتخزين بيانات الطالب المتعثر بعد إضافة درجات الرأفة
struct CurvedStudent {
    int id;
    string name;
    double oldAverage;
    double newAverage;
};

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    cout << fixed << setprecision(2);

    // ------------------------------------------------------
    // 3. نظام تشغيل الجامعة (Objects Array)
    // ------------------------------------------------------
    vector<Student> studentsList = {
        Student(1001, "أحمد علي",    {95, 88, 92}, 90),  // متفوق
        Student(1002, "منى حسن",     {45, 42, 48}, 80),  // متعثرة (متوسط 45)
        Student(1003, "عمر خالد",    {40, 44, 43}, 85),  // متعثر (متوسط 42.33)
        Student(1004, "سارة محمود",  {70, 65, 60}, 60)   // متوسط جيد لكن حضور ضعيف
    };

    // ------------------------------------------------------
    // 5-أ. تقرير لكل طالب في المصفوفة الرئيسية
    // ------------------------------------------------------
    cout << "============== تقرير نتائج الطلاب ==============\n";
    for (const Student& s : studentsList) {
        double avg = s.calculateAverage();
        cout << "رقم الجلوس: " << s.getId()
             << " | الاسم: " << s.getName()
             << " | المتوسط: " << avg
             << " | التقدير: " << s.getLetterGrade(avg)
             << " | الحضور: " << s.getAttendance() << "%"
             << " | النتيجة: " << s.getFinalResult() << "\n";
    }

    // ------------------------------------------------------
    // 4. لجنة الرأفة (Array Methods)
    // ------------------------------------------------------

    // الاستخراج (filter): الطلاب الذين متوسطهم بين 40 و 49
    vector<Student> strugglingStudents;
    copy_if(studentsList.begin(), studentsList.end(), back_inserter(strugglingStudents),
            [](const Student& s) {
                double avg = s.calculateAverage();
                return avg >= 40 && avg <= 49;
            });

    // المعالجة (map): إضافة 5 درجات رأفة لمتوسط كل طالب متعثر
    const double MERCY_POINTS = 5;
    vector<CurvedStudent> curvedStudents;
    transform(strugglingStudents.begin(), strugglingStudents.end(), back_inserter(curvedStudents),
              [MERCY_POINTS](const Student& s) {
                  double oldAvg = s.calculateAverage();
                  return CurvedStudent{s.getId(), s.getName(), oldAvg, oldAvg + MERCY_POINTS};
              });

    // التحليل (reduce): جمع المتوسطات الجديدة بعد التعديل
    double totalNewAverages = accumulate(curvedStudents.begin(), curvedStudents.end(), 0.0,
                                         [](double sum, const CurvedStudent& c) {
                                             return sum + c.newAverage;
                                         });

    // متوسط الرأفة مع "درع حماية" ضد القسمة على صفر
    double mercyAverage = curvedStudents.empty() ? 0.0
                                                 : totalNewAverages / curvedStudents.size();

    // ------------------------------------------------------
    // 5-ب. مصفوفة الطلاب المتعثرين الذين شملتهم الرأفة
    // ------------------------------------------------------
    cout << "\n========= الطلاب المتعثرون (لجنة الرأفة) =========\n";
    if (curvedStudents.empty()) {
        cout << "لا يوجد طلاب متعثرون.\n";
    } else {
        cout << "[\n";
        for (const CurvedStudent& c : curvedStudents) {
            cout << "  { id: " << c.id
                 << ", name: \"" << c.name << "\""
                 << ", oldAverage: " << c.oldAverage
                 << ", newAverage: " << c.newAverage << " }\n";
        }
        cout << "]\n";
    }

    // ------------------------------------------------------
    // 5-ج. متوسط درجات الطلاب المتعثرين بعد التعديل
    // ------------------------------------------------------
    cout << "\n================ متوسط الرأفة ================\n";
    if (curvedStudents.empty()) {
        cout << "لا يمكن حساب المتوسط (لا يوجد متعثرون) -> " << mercyAverage << "\n";
    } else {
        cout << "متوسط الطلاب المتعثرين بعد إضافة " << MERCY_POINTS
             << " درجات رأفة: " << mercyAverage << "\n";
    }

    return 0;
}
