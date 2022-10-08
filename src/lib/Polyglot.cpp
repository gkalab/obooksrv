#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "string.h"

#include "Polyglot.h"

using chess::Polyglot;

Polyglot::Polyglot(std::string const &fileName)
    : fileName(fileName) {
    FILE *f = fopen(fileName.c_str(), "rb");
    if (!f) {
        std::cerr << "Error reading File <" << fileName << ">" << std::endl;
        std::exit(1);
    }
    fclose(f);
}

entry_t entry_none = {
    0, 0, 0, 0
};

const char *promote_pieces = " nbrq";

static int int_from_file(FILE *file, int l, uint64_t *r) {
    for (int i = 0; i < l; i++) {
        int c = fgetc(file);
        if (c == EOF) {
            return 1;
        }
        (*r) = ((*r) << 8) + c;
    }
    return 0;
}

static int entry_from_file(FILE *file, entry_t *entry) {
    int ret;
    uint64_t r;
    ret = int_from_file(file, 8, &r);
    if (ret) {
        return 1;
    }
    entry->key = r;
    ret = int_from_file(file, 2, &r);
    if (ret) {
        return 1;
    }
    entry->move = r;
    ret = int_from_file(file, 1, &r);
    if (ret) {
        return 1;
    }
    entry->wins = r;
    ret = int_from_file(file, 1, &r);
    if (ret) {
        return 1;
    }
    entry->draws = r;
    ret = int_from_file(file, 4, &r);
    if (ret) {
        return 1;
    }
    entry->n = r;
    return 0;
}

int find_key(FILE *file, uint64_t key, entry_t *entry) {
    int first, last, middle;
    entry_t first_entry = entry_none, last_entry, middle_entry;
    first = -1;
    if (fseek(file, -16, SEEK_END)) {
        *entry = entry_none;
        entry->key = key + 1; //hack
        return -1;
    }
    last = ftell(file) / 16;
    entry_from_file(file, &last_entry);
    while (1) {
        if (last - first == 1) {
            *entry = last_entry;
            return last;
        }
        middle = (first + last) / 2;
        fseek(file, 16 * middle, SEEK_SET);
        entry_from_file(file, &middle_entry);
        if (key <= middle_entry.key) {
            last = middle;
            last_entry = middle_entry;
        } else {
            first = middle;
            first_entry = middle_entry;
        }
    }
}

static std::string move_to_string(uint16_t move) {
    char move_s[6];
    int f, fr, ff, t, tr, tf, p;
    f = (move >> 6) & 077;
    fr = (f >> 3) & 0x7;
    ff = f & 0x7;
    t = move & 077;
    tr = (t >> 3) & 0x7;
    tf = t & 0x7;
    p = (move >> 12) & 0x7;
    move_s[0] = ff + 'a';
    move_s[1] = fr + '1';
    move_s[2] = tf + 'a';
    move_s[3] = tr + '1';
    if (p) {
        move_s[4] = promote_pieces[p];
        move_s[5] = '\0';
    } else {
        move_s[4] = '\0';
    }
    if (!strcmp(move_s, "e1h1")) {
        snprintf(move_s, sizeof(move_s), "%s", "e1g1");
    } else  if (!strcmp(move_s, "e1a1")) {
        snprintf(move_s, sizeof(move_s), "%s", "e1c1");
    } else  if (!strcmp(move_s, "e8h8")) {
        snprintf(move_s, sizeof(move_s), "%s", "e8g8");
    } else  if (!strcmp(move_s, "e8a8")) {
        snprintf(move_s, sizeof(move_s), "%s", "e8c8");
    }
    return move_s;
}

std::string Polyglot::getMoves(std::string const &fen, bool asJson, int maxRows) {
    FILE *f = fopen(fileName.c_str(), "rb");
    if (!f) {
        return "error opening file";
    }
    uint64_t key = hasher.hashFen(fen.c_str());
    entry_t entry;
    int offset = find_key(f, key, &entry);
    if (entry.key != key) {
        if (asJson) {
            return toJson({}, maxRows);
        } else {
            return "";
        }
    }
    std::vector<entry_t> entries;
    entries.push_back(entry);
    fseek(f, 16 * (offset + 1), SEEK_SET);
    while (1) {
        int ret = entry_from_file(f, &entry);
        if (ret) {
            break;
        }
        if (entry.key != key) {
            break;
        }
        entries.push_back(entry);
    }
    fclose(f);

    if (asJson) {
        return toJson(entries, maxRows);
    } else {
        return toString(entries);
    }
}

std::string Polyglot::toString(std::vector<entry_t> entries) {
    std::stringstream result;
    for (entry_t entry : entries) {
        result << move_to_string(entry.move).c_str() << " ";
        result << static_cast<int>(entry.wins) << " ";
        result << static_cast<int>(entry.draws) << " ";
        result << static_cast<int>(entry.n) << std::endl;
    }
    return result.str();
}

std::string Polyglot::toJson(std::vector<entry_t> entries, int maxRows) {
    std::stringstream result;
    result << "{\"data\": [" << std::endl;
    size_t count = 0;
    for (entry_t entry : entries) {
        result << "{\"move\":\"" << move_to_string(entry.move) << "\",";
        result << "\"whitewins\":" << static_cast<int>(entry.wins) << ",";
        result << "\"draws\":" << static_cast<int>(entry.draws) << ",";
        result << "\"blackwins\":" << static_cast<int>(100 - entry.wins - entry.draws) << ",";
        result << "\"count\":" << static_cast<int>(entry.n) << "}";
        count++;
        if (maxRows > 0 && maxRows == count) {
            break;
        }
        if (count < entries.size()) {
            result << "," << std::endl;
        }
    }
    result << "]}";
    return result.str();
}
