#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

double Questions[166][5];
char AnswerString[167];

void AddScore(int q, double a1, double a2, double a3, double a4, double a5) {
    Questions[q][0] = a1;
    Questions[q][1] = a2;
    Questions[q][2] = a3;
    Questions[q][3] = a4;
    Questions[q][4] = a5;
}

double J(int QNum) {
    switch (AnswerString[--QNum]) {
	case 'A': return Questions[QNum][0];
	case 'B': return Questions[QNum][1];
	case 'C': return Questions[QNum][2];
	case 'D': return Questions[QNum][3];
	case 'E': return Questions[QNum][4];

        default:
            std::cerr << "What the hell..." << std::endl;
            throw;
    }
}

double Weight(bool Lesson, double Compare, double Lowest, double NewLowest, double Highest, double NewHighest, double NextFrom, double NextTo, double NewNext) {
    if (Lesson) {
	if (Compare <= Lowest)
	    return NewLowest;
	else if (Compare >= Highest)
	    return NewHighest;
    } else {
	if (Compare <= Lowest)
	    return NewLowest;
	else if (Compare >= NextFrom && Compare <= NextTo)
	    return NewNext;
	else if (Compare >= Highest)
	    return NewHighest;
    }

    return 0;
}

const char *GetColor(double analyze, double lowest, double highest) {
    if (analyze < lowest)
        return "Red";
    else if (analyze > highest)
        return "Green";
    else
        return "Yellow";
}

int main(int argc, const char *argv[]) {
    if (argc != 2 || (
        strcmp(argv[1], "mstrcuts") != 0 &&
        strcmp(argv[1], "othersal") != 0 &&
        strcmp(argv[1], "regis") != 0 &&
        strcmp(argv[1], "smrtstyl") != 0 &&
        strcmp(argv[1], "suprcuts") != 0 &&
        strcmp(argv[1], "tradesec") != 0
    )) {
        std::cerr << "argument must be one of:" << std::endl;
        std::cerr << "  mstrcuts" << std::endl;
        std::cerr << "  othersal" << std::endl;
        std::cerr << "  regis" << std::endl;
        std::cerr << "  smrtstyl" << std::endl;
        std::cerr << "  suprcuts" << std::endl;
        std::cerr << "  tradesec" << std::endl;
        return -1;
    }

    std::string line;
    std::getline(std::cin, line);

    typedef std::map<std::string, size_t> names_t;
    names_t names;

    for (size_t begin(0), i(0);; ++i) {
        size_t comma(line.find(',', begin));
        if (comma == std::string::npos) {
            names.insert(names_t::value_type(line.substr(begin), i));
            break;
        }

        names.insert(names_t::value_type(line.substr(begin, comma - begin), i));
        begin = comma + 1;
    }

    names_t::const_iterator SPEC_SALON(names.find("SPEC_SALON"));

    while (std::getline(std::cin, line)) { {
        std::vector<std::string> fields;

        for (size_t begin(0);;) {
            size_t comma(line.find(',', begin));
            if (comma == std::string::npos) {
                fields.push_back(line.substr(begin));
                break;
            }

            fields.push_back(line.substr(begin, comma - begin));
            begin = comma + 1;
        }

        if (fields.size() != names.size()) {
            std::cerr << "bad field count! << (" << fields.size() << " != " << names.size() << ")" << std::endl;
            goto next;
        }

        std::string SuperName(fields[names["SUPER_NAME"]]);
        std::string FirstName(fields[names["FIRST_NAME"]]);
        std::string LastName(fields[names["LAST_NAME"]]);
        std::string SSN(fields[names["SSN"]]);

        std::string Date(fields[names["DATE"]]);
        std::string StoreNumber(fields[names["STORE_NUM"]]);

        std::string salon;
        if (SPEC_SALON != names.end())
            salon = fields[SPEC_SALON->second];

        for (unsigned i(0); i != 166; ++i) {
            char name[6];
            sprintf(name, "QQ%.3d", i + 1);

            const std::string &field(fields[names[name]]);

            if (field.empty()) {
                std::cerr << "missing answer!" << std::endl;
                goto next;
            }

            if (i > 143)
                AnswerString[i] = field[0];
            else if (field == "SD")
                AnswerString[i] = 'A';
            else if (field == "D")
                AnswerString[i] = 'B';
            else if (field == "N")
                AnswerString[i] = 'C';
            else if (field == "A")
                AnswerString[i] = 'D';
            else if (field == "SA")
                AnswerString[i] = 'E';
            else {
                std::cerr << "weird answer!" << std::endl;
                goto next;
            }
        }

        AnswerString[166] = '\0';

	double jhect, jwkjoy, jtuff, jamb, jperprb, jdeltrnq, jpride, jemoctrl, jenrgy, jcreatv, jclnwk, jwinatt, jnopull, jfake, j2tuff, j2pride, j2wkjoy, j2creatv;

	//Values for each answer is stored in the Questions array, which
	//   is populated below
	for (int i(0); i != 144; i++)
	    switch (i + 1) {
		case 1: case 2: case 3: case 5: case 8: case 13: case 15: case 19: case 20: case 21: case 22: case 23: case 28: case 30: case 31: case 32: case 34: case 36: case 37: case 38: case 42: case 44: case 50: case 51: case 55: case 56: case 58: case 59: case 62: case 63: case 66: case 67: case 69: case 70: case 74: case 75: case 80: case 83: case 84: case 86: case 88: case 89: case 91: case 93: case 94: case 96: case 97: case 99: case 103: case 104: case 108: case 112: case 113: case 119: case 121: case 124: case 126: case 127: case 128: case 134: case 135: case 139: case 142: case 144:
		    AddScore(i, 0, 0.25, 0.5, 0.75, 1);
		break;

		default:
		    AddScore(i, 1, 0.75, 0.5, 0.25, 0);
		break;
	    }

	for (int i(144); i != 166; i++)
	    switch (i + 1) {
		case 145: case 146: case 148: case 149: case 151: case 154: case 156: case 159: case 160: case 163:
		    AddScore(i, 0, 1, 0, 0, 0);
		break;

		default:
		    AddScore(i, 1, 0, 0, 0, 0);
		break;
	    }

	jhect = J(61) + J(95) + J(101) + J(110) + J(115) + J(125) + J(135) + J(140) + J(82) + J(71) + J(57) + J(84) + J(1) + J(5) + J(12) + J(14) + J(15) + J(21) + J(32) + J(37) + J(50);
	jwkjoy = J(10) + J(27) + J(78) + J(106) + J(147) + J(16) + J(33) + J(56) + J(70) + J(83) + J(111) + J(120) + J(126) + J(157);
	jtuff = J(42) + J(63) + J(138) + J(145) + J(149) + J(150) + J(154) + J(158) + J(163) + J(164);
	jamb = J(91) + J(151) + J(156) + J(159) + J(161) + J(166);
	jperprb = J(19) + J(51) + J(85) + J(129) + J(141);
	jdeltrnq = J(8) + J(45) + J(65) + J(77) + J(105) + J(117) + J(123) + J(148) + J(155);
	jpride = J(66) + J(28) + J(31) + J(53) + J(58) + J(81) + J(94) + J(142);
	jemoctrl = J(7) + J(26) + J(43) + J(64) + J(90) + J(104) + J(122) + J(146) + J(160) + J(17) + J(44) + J(116) + J(130) + J(133);
	jenrgy = J(6) + J(24) + J(62) + J(88) + J(103) + J(136);
	jcreatv = J(40) + J(74) + J(86) + J(97) + J(132) + J(137) + J(23) + J(112) + J(127);
	jclnwk = J(152) + J(162);
	jwinatt = J(38) + J(121) + J(34) + J(36) + J(52) + J(144) + J(99) + J(114) + J(73) + J(60) + J(29) + J(87) + J(131) + J(124) + J(35) + J(4) + J(9) + J(20) + J(46) + J(68);
	jnopull = J(92) + J(107) + J(118) + J(79);

	jfake = J(3) + J(18) + J(39) + J(72) + J(102);

	j2tuff = jtuff + J(139) + J(25);
	j2pride = jpride + J(22);
	j2wkjoy = jwkjoy + J(89);
	j2creatv = jcreatv + J(108);

        double creat;
        double prde;
        double amb;
        double tuff;
        double wkjy;
        double clnw;
        double dt;
        double emo;
        double engy;
        double hec;
        double win;
        double npul;
        double scr;
        double scrcr;
        const char *Color;

        if (
            strcmp(argv[1], "mstrcuts") == 0 ||
            strcmp(argv[1], "regis") == 0
        ) {
	    double mal2tuf, mal2crtv, mal2prde, mal2wkjy, mal2amb, mal2clnw, mal2dt, mal2emo, mal2engy, mal2hec, mal2npul, mal2win, mal2sc, mal2sccf;
	    mal2tuf = Weight(false, j2tuff, 6.25, -55, 8, 8, 6.5, 7.75, -15);
	    mal2crtv = Weight(true, j2creatv, 6.5, -16, 6.75, 6, 0, 0, 0);
	    mal2prde = Weight(true, j2pride, 6, -25, 6.25, 5, 0, 0, 0);
	    mal2wkjy = Weight(true, j2wkjoy, 10, -26, 10.25, 10, 0, 0, 0);

	    mal2amb = 0;
	    if (jamb <= 2.25)
	        mal2amb = -25;
	    else if (jamb >= 2.5 && jamb <= 4)
	        mal2amb = -8;
	    else if (jamb >= 4.25 && jamb <= 4.5)
	        mal2amb = 0;
	    else if (jamb >= 4.75)
	        mal2amb = 20;

	    mal2clnw = Weight(true, jclnwk, 1, -18, 2, 13, 0, 0, 0);
	    mal2dt = Weight(true, jdeltrnq, 6.25, -10, 6.5, 10, 0, 0, 0);
	    mal2emo = Weight(false, jemoctrl, 8.75, -17, 9.75, 17, 9, 9.5, -9);
	    mal2engy = Weight(false, jenrgy, 2.75, -12, 4, 26, 3, 3.75, -8);
	    mal2hec = Weight(false, jhect, 12.25, -11, 14.75, 15, 12.5, 14.5, 0);
	    mal2npul = Weight(false, jnopull, 2.5, -16, 3.25, 9, 2.75, 3, 0);
	    mal2win = Weight(false, jwinatt, 13, -39, 14.75, 17, 13.25, 14.5, -6);

            mal2sc = mal2amb + mal2clnw + mal2crtv + mal2dt + mal2emo + mal2engy + mal2hec + mal2npul + mal2prde + mal2tuf + mal2win + mal2wkjy;
            mal2sccf = mal2sc + 100 - 11 * jfake;
            creat = mal2crtv;
            prde = mal2prde;
            amb = mal2amb;
            tuff = mal2tuf;
            wkjy = mal2wkjy;
            clnw = mal2clnw;
            dt = mal2dt;
            emo = mal2emo;
            engy = mal2engy;
            hec = mal2hec;
            win = mal2win;
            npul = mal2npul;
            scr = mal2sc;
            scrcr = mal2sccf;
            Color = GetColor(mal2sccf, 21, 40);
        } else if (
            strcmp(argv[1], "othersal") == 0 ||
            strcmp(argv[1], "smrtstyl") == 0 ||
            strcmp(argv[1], "suprcuts") == 0
        ) {
	    double st2creat, st2tuf, st2wkjy, st2amb, st2clnw, st2dt, st2emo, st2engy, st2hec, st2win, st2sc, st2scrc;
	    st2creat = Weight(true, j2creatv, 6.25, -7, 6.5, 3, 0, 0, 0);
	    st2tuf = Weight(false, j2tuff, 8, -20, 10, 40, 8.25, 9.75, -10);
	    st2wkjy = Weight(true, j2wkjoy, 10.25, -17, 10.5, 9, 0, 0, 0);
	    st2amb = Weight(true, jamb, 3, -25, 3.25, 11, 0, 0, 0);
	    st2clnw = Weight(true, jclnwk, 1, -3, 2, 3, 0, 0, 0);
	    st2dt = Weight(true, jdeltrnq, 5.75, -16, 6, 10, 0, 0, 0);
	    st2emo = Weight(true, jemoctrl, 8.25, -8, 8.5, 4, 0, 0, 0);
	    st2engy = Weight(true, jenrgy, 2.75, -12, 3, 7, 0, 0, 0);
	    st2hec = Weight(true, jhect, 12, -12, 12.25, 4, 0, 0, 0);
	    st2win = Weight(false, jwinatt, 12.5, -41, 14, 15, 12.75, 13.75, 0);

            st2sc = st2amb + st2clnw + st2creat + st2dt + st2emo + st2engy + st2hec + st2tuf + st2win + st2wkjy;
            st2scrc = st2sc + 100 - 11 * jfake;
            creat = st2creat;
            prde = 0;
            amb = st2amb;
            tuff = st2tuf;
            wkjy = st2wkjy;
            clnw = st2clnw;
            dt = st2dt;
            emo = st2emo;
            engy = st2engy;
            hec = st2hec;
            win = st2win;
            npul = 0;
            scr = st2sc;
            scrcr = st2scrc;
            Color = GetColor(st2scrc, 40, 60);
        } else if (strcmp(argv[1], "tradesec") == 0) {
	    double tr2creat, tr2prde, tr2tuff, tr2wkjy, tr2clnw, tr2dt, tr2emo, tr2engy, tr2hec, tr2win, tr2scr, tr2scrcr;
	    tr2creat = Weight(true, j2creatv, 6, -20, 6.25, 5, 0, 0, 0);
	    tr2prde = Weight(true, j2pride, 6, -45, 6.25, 15, 0, 0, 0);
	    tr2tuff = Weight(true, j2tuff, 10.25, -14, 10.5, 30, 0, 0, 0);
	    tr2wkjy = Weight(true, j2wkjoy, 10.5, -17, 10.75, 19, 0, 0, 0);
	    tr2clnw = Weight(true, jclnwk, 0, -24, 1, 6, 0, 0, 0);
	    tr2dt = Weight(true, jdeltrnq, 6, -30, 6.25, 24, 0, 0, 0);
	    tr2emo = Weight(true, jemoctrl, 8.5, -8, 8.75, 4, 0, 0, 0);
	    tr2engy = Weight(true, jenrgy, 2.5, -38, 2.75, 8, 0, 0, 0);
	    tr2hec = Weight(true, jhect, 11, -72, 11.25, 4, 0, 0, 0);
	    tr2win = Weight(true, jwinatt, 13, -43, 13.25, 12, 0, 0, 0);

            tr2scr = tr2clnw + tr2creat + tr2dt + tr2emo + tr2engy + tr2hec + tr2prde + tr2tuff + tr2win + tr2wkjy;
            tr2scrcr = tr2scr + 100 - 19 * jfake;
            creat = tr2creat;
            prde = tr2prde;
            amb = 0;
            tuff = tr2tuff;
            wkjy = tr2wkjy;
            clnw = tr2clnw;
            dt = tr2dt;
            emo = tr2emo;
            engy = tr2engy;
            hec = tr2hec;
            win = tr2win;
            npul = 0;
            scr = tr2scr;
            scrcr = tr2scrcr;
            Color = GetColor(tr2scrcr, 25, 40);
        } else throw;

        std::cout << "\"" << FirstName << "\" \"" << LastName << "\" " << SSN << " " << Color << " \"" << SuperName << "\" " << jhect << " " << jwkjoy << " " << jtuff << " " << jamb << " " << jperprb << " " << jdeltrnq << " " << jpride << " " << jemoctrl << " " << jenrgy << " " << jcreatv << " " << jclnwk << " " << jwinatt << " " << jnopull << " " << jfake << " " << j2tuff << " " << j2pride << " " << j2wkjoy << " " << j2creatv << " " << creat << " " << prde << " " << amb << " " << tuff << " " << wkjy << " " << clnw << " " << dt << " " << emo << " " << engy << " " << hec << " " << win << " " << npul << " " << scr << " " << scrcr << " " << AnswerString << " \"" << Date << "\" \"" << StoreNumber << "\"";
        if (!salon.empty())
            std::cout << " \"" << salon << "\"";
        std::cout << std::endl;

        }
        next:;
    }

    return 0;
}
