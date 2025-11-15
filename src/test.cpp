#include "test.h"
#include "Transducer.h"
#include "FileUtils.h"
#include "TransducerCreator.h"
#include "TestUtils.h"
#include <chrono>
#include <cstring>
#include <random> 
#include <algorithm> 
#include <cassert>

//COPY PASTE:
    // SHUFFLE:
    // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // std::shuffle(d2.begin(), d2.end(), std::default_random_engine(seed));

    // auto start1 = std::chrono::high_resolution_clock::now();
    // auto end1 = std::chrono::high_resolution_clock::now();

    // // Calculate the duration
    // std::chrono::duration<double, std::milli> duration1 = end1 - start1;
    // std::cout << "Result :  " << duration1.count() / 1000 << " seconds." << std::endl;
    

// Helper function to compare two Elements
bool elementsEqual(const E& a, const E& b) {
    return a.second == b.second && a.first == b.first;
}

// Returns elements in v1 that are NOT in v2
std::vector<E> differenceVector(
    const std::vector<E>& v1,
    const std::vector<E>& v2)
{
    std::vector<E> result;

    for (const auto& el1 : v1) {
        // Check if el1 is NOT found in v2
        auto it = std::find_if(v2.begin(), v2.end(),
            [&](const E& el2) {
                return elementsEqual(el1, el2);
            });
        if (it == v2.end()) {
            result.push_back(el1);
        }
    }

    return result;
}

// Returns vector with Vs from both v1 and v2 (concatenation)
std::vector<E> unionVector(
    const std::vector<E>& v1,
    const std::vector<E>& v2)
{
    std::vector<E> result = v1;
    result.insert(result.end(), v2.begin(), v2.end());
    return result;
}


void testAngel() {
    const char* dict_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/angel/dict.csv";
    const char* delete_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/angel/delete.csv";
    const char* add_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/angel/add.csv";
    auto dict = read_utf8_csv(dict_file);
    auto del = read_utf8_csv(delete_file);
    auto add = read_utf8_csv(add_file);

    Transducer t;
    std::map<StateSignature, State> minimized;

    std::tie(t, minimized) = fromSortedDictionary(dict);

    assert(t.statesCount == 8111085);
    assert(t.getTransitionsCount() == 8210803);
    assert(t.finalStates.size() == 134);

    for (auto p : del) {
        deleteWord(t, p.first, minimized);
    }

    assert(t.statesCount == 8088387);
    assert(t.getTransitionsCount() == 8187807);
    assert(t.finalStates.size() == 134);

    for (auto p : add) {
        addArbitraryWord(t, p.first, p.second, minimized);
    }

    assert(t.statesCount == 8099606);
    assert(t.getTransitionsCount() == 8199175);
    assert(t.finalStates.size() == 134);

    std::cout<< "Angel test passed" << std::endl;
}

void myTest1() {
    const char* total_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/input_PM.csv";
    const char* part1_shuff_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part1_shuffled.csv";
    const char* part1_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part1.csv";
    const char* part2_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part2.csv";
    // const char* part2_shuffled_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part2_shuffled.csv";
    auto total = read_utf8_csv(total_file);
    auto del = read_utf8_csv(part1_shuff_file);
    auto after_delete = read_utf8_csv(part2_file);

    Transducer t_total;
    std::map<StateSignature, State> min_total;
    std::tie(t_total, min_total) = fromSortedDictionary(total);

    size_t initial_states = t_total.statesCount;
    size_t initial_transitions = t_total.getTransitionsCount();
    size_t initial_final = t_total.finalStates.size();

    for (auto p : del) {
        deleteWord(t_total, p.first, min_total);
    }

    Transducer t_part2;
    std::map<StateSignature, State> min_part2;
    std::tie(t_part2, min_part2) = fromSortedDictionary(after_delete);

    for (auto p : after_delete) {
        assert(t_total.traverse(p.first) == p.second);
        assert(t_part2.traverse(p.first) == p.second);
    }

    assert(t_total.statesCount == t_part2.statesCount);
    assert(t_total.getTransitionsCount() == t_part2.getTransitionsCount());
    assert(t_total.finalStates.size() == t_part2.finalStates.size());

    for (auto p : del) {
        addArbitraryWord(t_total, p.first, p.second, min_total);
    }

    assert(t_total.statesCount == initial_states);
    assert(t_total.getTransitionsCount() == initial_transitions);
    assert(t_total.finalStates.size() == initial_final);

    for (auto p : total) {
        assert(t_total.traverse(p.first) == p.second);
    }

    std::cout << "MyTest1 passed" << std::endl;
}

void myTest2() {
    const char* total_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/input_PM.csv";
    const char* part1_shuff_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part1_shuffled.csv";
    const char* part1_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part1.csv";
    const char* part2_shuffled_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part2_shuffled.csv";
    auto total = read_utf8_csv(total_file);
    auto del1 = read_utf8_csv(part1_shuff_file);
    auto del2 = read_utf8_csv(part2_shuffled_file);

    Transducer t_total;
    std::map<StateSignature, State> min_total;
    std::tie(t_total, min_total) = fromSortedDictionary(total);

    size_t initial_states = t_total.statesCount;
    size_t initial_transitions = t_total.getTransitionsCount();
    size_t initial_final = t_total.finalStates.size();

    for (auto p : del1) {
        deleteWord(t_total, p.first, min_total);
    }

    for (auto p : del2) {
        deleteWord(t_total, p.first, min_total);
    }    
    assert(t_total.statesCount == 1);
    assert(t_total.getTransitionsCount() == 0);
    assert(t_total.finalStates.size() == 0);
    std::cout << "MyTest2 passed" << std::endl;
}

void myTest3() {
    const char* total_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/dict.csv";
    const char* part1_shuff_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/part1_shuffled.csv";
    const char* part1_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/part1.csv";
    const char* part2_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/part2.csv";
    // const char* part2_shuffled_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part2_shuffled.csv";
    auto total = read_utf8_csv(total_file);
    auto del = read_utf8_csv(part1_shuff_file);
    auto after_delete = read_utf8_csv(part2_file);

    Transducer t_total;
    std::map<StateSignature, State> min_total;
    std::tie(t_total, min_total) = fromSortedDictionary(total);

    size_t initial_states = t_total.statesCount;
    size_t initial_transitions = t_total.getTransitionsCount();
    size_t initial_final = t_total.finalStates.size();

    for (auto p : del) {
        deleteWord(t_total, p.first, min_total);
    }

    Transducer t_part2;
    std::map<StateSignature, State> min_part2;
    std::tie(t_part2, min_part2) = fromSortedDictionary(after_delete);

    for (auto p : after_delete) {
        assert(t_total.traverse(p.first) == p.second);
        assert(t_part2.traverse(p.first) == p.second);
    }

    assert(t_total.statesCount == t_part2.statesCount);
    assert(t_total.getTransitionsCount() == t_part2.getTransitionsCount());
    assert(t_total.finalStates.size() == t_part2.finalStates.size());

    for (auto p : del) {
        addArbitraryWord(t_total, p.first, p.second, min_total);
    }

    assert(t_total.statesCount == initial_states);
    assert(t_total.getTransitionsCount() == initial_transitions);
    assert(t_total.finalStates.size() == initial_final);

    for (auto p : total) {
        assert(t_total.traverse(p.first) == p.second);
    }

    std::cout << "MyTest3 passed" << std::endl;
}

void myTest4() {
    const char* total_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/dict.csv";
    const char* part1_shuff_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/part1_shuffled.csv";
    const char* part1_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/part1.csv";
    const char* part2_shuffled_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test2/part2_shuffled.csv";
    auto total = read_utf8_csv(total_file);
    auto del1 = read_utf8_csv(part1_shuff_file);
    auto del2 = read_utf8_csv(part2_shuffled_file);

    Transducer t_total;
    std::map<StateSignature, State> min_total;
    std::tie(t_total, min_total) = fromSortedDictionary(total);

    size_t initial_states = t_total.statesCount;
    size_t initial_transitions = t_total.getTransitionsCount();
    size_t initial_final = t_total.finalStates.size();

    for (auto p : del1) {
        deleteWord(t_total, p.first, min_total);
    }

    for (auto p : del2) {
        deleteWord(t_total, p.first, min_total);
    }    
    assert(t_total.statesCount == 1);
    assert(t_total.getTransitionsCount() == 0);
    assert(t_total.finalStates.size() == 0);
    std::cout << "MyTest4 passed" << std::endl;
}
void myTest5() {
    const char* total_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/dict.csv";
    const char* part1_shuff_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/part1_shuffled.csv";
    // const char* part1_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/part1.csv";
    const char* part2_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/part2.csv";
    // const char* part2_shuffled_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test1/part2_shuffled.csv";
    auto total = read_utf8_csv(total_file);
    auto del = read_utf8_csv(part1_shuff_file);
    auto after_delete = read_utf8_csv(part2_file);

    Transducer t_total;
    std::map<StateSignature, State> min_total;
    std::tie(t_total, min_total) = fromSortedDictionary(total);

    size_t initial_states = t_total.statesCount;
    size_t initial_transitions = t_total.getTransitionsCount();
    size_t initial_final = t_total.finalStates.size();

    for (auto p : del) {
        deleteWord(t_total, p.first, min_total);
    }

    Transducer t_part2;
    std::map<StateSignature, State> min_part2;
    std::tie(t_part2, min_part2) = fromSortedDictionary(after_delete);

    for (auto p : after_delete) {
        assert(t_total.traverse(p.first) == p.second);
        assert(t_part2.traverse(p.first) == p.second);
    }

    assert(t_total.statesCount == t_part2.statesCount);
    assert(t_total.getTransitionsCount() == t_part2.getTransitionsCount());
    assert(t_total.finalStates.size() == t_part2.finalStates.size());

    for (auto p : del) {
        addArbitraryWord(t_total, p.first, p.second, min_total);
    }

    assert(t_total.statesCount == initial_states);
    assert(t_total.getTransitionsCount() == initial_transitions);
    assert(t_total.finalStates.size() == initial_final);

    for (auto p : total) {
        assert(t_total.traverse(p.first) == p.second);
    }

    std::cout << "MyTest5 passed" << std::endl;
}

void myTest6() {
    const char* total_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/dict.csv";
    const char* part1_shuff_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/part1_shuffled.csv";
    const char* part1_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/part1.csv";
    const char* part2_shuffled_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test3/part2_shuffled.csv";
    auto total = read_utf8_csv(total_file);
    auto del1 = read_utf8_csv(part1_shuff_file);
    auto del2 = read_utf8_csv(part2_shuffled_file);

    Transducer t_total;
    std::map<StateSignature, State> min_total;
    std::tie(t_total, min_total) = fromSortedDictionary(total);

    size_t initial_states = t_total.statesCount;
    size_t initial_transitions = t_total.getTransitionsCount();
    size_t initial_final = t_total.finalStates.size();

    for (auto p : del1) {
        deleteWord(t_total, p.first, min_total);
    }

    for (auto p : del2) {
        deleteWord(t_total, p.first, min_total);
    }    
    assert(t_total.statesCount == 1);
    assert(t_total.getTransitionsCount() == 0);
    assert(t_total.finalStates.size() == 0);
    std::cout << "MyTest5 passed" << std::endl;
}

void testPM() {
    const char* build_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test0/build.csv";
    const char* csv11_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test0/1-1.csv";
    const char* csv12_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test0/1-2.csv";
    const char* add1_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test0/add1.csv";
    const char* add2_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test0/add2.csv";

    auto build = read_utf8_csv(build_file);
    auto csv11 = read_utf8_csv(csv11_file);
    auto csv12 = read_utf8_csv(csv12_file);
    auto add1 = read_utf8_csv(add1_file);
    auto add2 = read_utf8_csv(add2_file);

    Transducer t;
    std::map<StateSignature, State> min;
    std::tie(t, min) = fromSortedDictionary(build);
    std::cout << "Initial built: " << std::endl;
    t.compactPrint();

    for (auto p : csv11) {
        deleteWord(t, p.first, min);
    }
    std::cout << "After deleting 1-1: " << std::endl;
    t.compactPrint();

    for (auto p : csv12) {
        deleteWord(t, p.first, min);
    }
    std::cout << "After deleting 1-2: " << std::endl;
    t.compactPrint();

    for (auto p : add1) {
        addArbitraryWord(t, p.first, p.second, min);
    }
    std::cout << "After adding add1: " << std::endl;
    t.compactPrint();

    for (auto p : add2) {
        addArbitraryWord(t, p.first, p.second, min);
    }
    std::cout << "After adding add1: " << std::endl;
    t.compactPrint();

    Transducer t2;
    std::map<StateSignature, State> min2;
    std::tie(t2, min2) = fromSortedDictionary(add1);
    std::cout << "Add1 with from sorted: " << std::endl;
    t2.compactPrint();

    Transducer t3;
    std::map<StateSignature, State> min3;
    std::tie(t3, min3) = fromSortedDictionary(add2);
    std::cout << "Add2 with from sorted: " << std::endl;
    t3.compactPrint();
}


#define FILENAME1 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/test2.csv"
#define FILENAME2 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/1-1.txt"
#define FILENAME3 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/1-2.txt"
#define FILENAME4 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/test3.csv"
#define FILENAME5 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/add1.csv"
#define FILENAME6 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/add2.csv"
#define FILENAME7 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/test4.csv"
#define FILENAME8 "/home/deyan/Desktop/min-subseq-transducer/test_files/exam/ph2.csv"


void test0() {
    auto dict = read_utf8_csv(FILENAME1);

    auto del1 = read_utf8_csv(FILENAME2);
    auto del2 = read_utf8_csv(FILENAME3);
    auto test3 = read_utf8_csv(FILENAME4);
    auto add1 = read_utf8_csv(FILENAME5);
    auto add2 = read_utf8_csv(FILENAME6);
    auto dict2 = read_utf8_csv(FILENAME7);
    auto ph = read_utf8_csv(FILENAME8);


    // std::vector<E> d01 (dict.begin(), dict.begin() + 10000);

    Transducer t1;
    std::map<StateSignature, State> minimized1;

    std::tie(t1, minimized1) = fromSortedDictionary(dict);
    std::cout << "First build: " << std::endl;
    t1.compactPrint();

    for (auto p : del1) {
        deleteWord(t1, p.first, minimized1);
    }
    std::cout << "After deleting 1-1: " << std::endl;
    t1.compactPrint();

    for (auto p : test3) {
        // std::cerr << t1.traverse(p.first) << "  " << p.second << std::endl;
        assert(t1.traverse(p.first) == p.second);
    }

    for (auto p : del2) {
        deleteWord(t1, p.first, minimized1);
    }
    std::cout << "After deleting 1-2: " << std::endl;

    t1.compactPrint();

    for (auto p : add2) {
        addArbitraryWord(t1, p.first, p.second, minimized1);
    }

    std::cout << "After add2: " << std::endl;

    t1.compactPrint();

    for (auto p : add1) {
        addArbitraryWord(t1, p.first, p.second, minimized1);
    }
    
    std::cout << "After add2: " << std::endl;

    t1.compactPrint();

    
    Transducer t2;
    std::map<StateSignature, State> minimized2;

    std::tie(t2, minimized2) = fromSortedDictionary(ph);
    
    std::cout << "Should be like after delete 1-1: " << std::endl;

    t2.compactPrint();


    Transducer t4;
    std::map<StateSignature, State> minimized4;

    std::tie(t4, minimized4) = fromSortedDictionary(add1);
    
    std::cout << "Should be like after add1: " << std::endl;

    t4.compactPrint();

}




void test1() {
    auto ph = read_utf8_csv(FILENAME8);
    int diff = 2;
    std::vector<E> first(ph.begin(), ph.begin() + diff);
    std::vector<E> second(ph.begin() + diff, ph.end());

    // 347 - wrong 348 - correct
    Transducer t1;
    std::map<StateSignature, State> minimized1;

    std::tie(t1, minimized1) = fromSortedDictionary(ph);

    std::cout << "From sorted: " << std::endl;
    t1.compactPrint();

    Transducer t2;
    std::map<StateSignature, State> minimized2;

    std::tie(t2, minimized2) = fromSortedDictionary(first);
    for (auto p : second) {
        // std::cout << "adding: "; print_char32_vector(p.first); std::cout << std::endl;
        addArbitraryWord(t2, p.first, p.second, minimized2);
    }
    
    std::cout << "From unsorted: " << std::endl;

    t2.compactPrint();

    // std::cout << "Traversing: " << std::endl;
    for (auto p : ph) {
        // print_char32_vector(p.first);
        // std::cout<<','<<p.second<<std::endl;
        assert(t1.traverse(p.first) == p.second);
        assert(t2.traverse(p.first) == p.second);
    }


}

void test3() {
    auto dict = read_utf8_csv(FILENAME1);
    int diff = 2;
    std::vector<E> first(dict.begin(), dict.begin() + diff);
    std::vector<E> second(dict.begin() + diff, dict.end());

    // 347 - wrong 348 - correct
    Transducer t1;
    std::map<StateSignature, State> minimized1;

    std::tie(t1, minimized1) = fromSortedDictionary(dict);

    // std::cout << "From sorted: " << std::endl;
    // t1.compactPrint();

    // Transducer t2;
    // std::map<StateSignature, State> minimized2;

    // std::tie(t2, minimized2) = fromSortedDictionary(first);
    // for (auto p : second) {
    //     // std::cout << "adding: "; print_char32_vector(p.first); std::cout << std::endl;
    //     addArbitraryWord(t2, p.first, p.second, minimized2);
    // }
    
    // std::cout << "From unsorted: " << std::endl;

    // t2.compactPrint();

    // std::cout << "Traversing: " << std::endl;
    // for (auto p : getLanguage(t1)) {
    //     print_char32_vector(p.first);
    //     std::cout<<','<<p.second<<std::endl;
    // }

    for (auto p : dict) {

        Nat res = t1.traverse(p.first);
        if (res != p.second) {
            print_char32_vector(p.first); std::cout << std::endl;
            std::cout<<"t1 res: " << res << "  Original:  " << p.second << std::endl;
        }
    
    }

}

void exam1() {
    const char* build_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test4/b-start.csv";
    const char* add_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test4/add.csv";
    const char* del_file = "/home/deyan/Desktop/min-subseq-transducer/test_files/test4/del.csv";

    auto build = read_utf8_csv(build_file);
    auto add = read_utf8_csv(add_file);
    auto del = read_utf8_csv(del_file);

    auto start1 = std::chrono::high_resolution_clock::now();



    Transducer t;
    std::map<StateSignature, State> min;
    std::tie(t, min) = fromSortedDictionary(build);
    std::cout << "Initial built: " << std::endl;
    t.compactPrint();

    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration1 = end1 - start1;
    std::cout << "Initial build took:  " << duration1.count() / 1000 << " seconds." << std::endl;

    auto start2 = std::chrono::high_resolution_clock::now();
    for (auto p : add) {
        addArbitraryWord(t, p.first, p.second, min);
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration2 = end2 - start2;
    t.compactPrint();
    std::cout << "Initial build took:  " << duration2.count() / 1000 << " seconds." << std::endl;


    auto start3 = std::chrono::high_resolution_clock::now();
    for (auto p : del) {
        deleteWord(t, p.first, min);
    }
    auto end3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration3 = end3 - start3;
    t.compactPrint();
    std::cout << "Initial build took:  " << duration3.count() / 1000 << " seconds." << std::endl;

}