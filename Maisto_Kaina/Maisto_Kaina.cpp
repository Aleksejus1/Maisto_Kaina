#include "stdafx.h"

class maistas{
public:
	void nustatykPorcijuKieki(float kiekis){ porcijuKiekis = kiekis; atnaujinti = true; }
	std::string
		pavadinimas;
	bool
		atnaujinti = false;
	float
		porcijuKiekis = 1.f,
		kaina = 0.f;
};
class produktas : public maistas{
public:
	void nustatykProduktoNevienetineKaina(float kaina){ produktoNevienetineKaina = kaina; atnaujinti = true; }
	void nustatykNaudojamaPorcijuKiekis(float kiekis){ naudojamasPorcijuKiekis = kiekis; atnaujinti = true; }
	float gaukKaina(){ if (!menesineKaina) { suskaiciuokKaina(); return kaina; } else return 0.f; }
	float gaukNaudojamaPorcijuKieki(){ return naudojamasPorcijuKiekis; }
	bool menesineKaina = false;
private:
	float
		produktoNevienetineKaina = 0.f,
		naudojamasPorcijuKiekis = 1.f;
	void suskaiciuokKaina(){
		if (atnaujinti){
			atnaujinti = false;
			kaina = produktoNevienetineKaina / porcijuKiekis * naudojamasPorcijuKiekis;
		}
	}
};
class patiekalas : public maistas{
public:
	std::vector<patiekalas> patiekalai;
	std::vector<produktas> produktai;
	float gaukKaina(){ suskaiciuokKaina(); return kaina; }
	void pridekProdukta(produktas _produktas, float porcijuKiekis){
		atnaujinti = true;
		_produktas.nustatykNaudojamaPorcijuKiekis(porcijuKiekis);
		produktai.push_back(_produktas);
	}
	void pridekPatiekala(patiekalas _patiekalas, float porcijuKiekis){
		atnaujinti = true;
		_patiekalas.nustatykPorcijuKieki(porcijuKiekis);
		patiekalai.push_back(_patiekalas);
	}
	std::vector<produktas> gautiSanaudas(){
		std::vector<produktas> sanaudos;
		for (int i = 0; i < (int)produktai.size(); i++){
			sanaudos.push_back(produktai[i]);
		}
		for (int i = 0; i < (int)patiekalai.size(); i++){
			std::vector<produktas> temp = patiekalai[i].gautiSanaudas();
			sanaudos.insert(sanaudos.end(), temp.begin(), temp.end());
		}
		return sanaudos;
	}
private:
	void suskaiciuokKaina(){
		if (atnaujinti){
			atnaujinti = false;
			kaina = 0.f;
			for (int i = 0; i < (int)patiekalai.size(); i++){
				kaina += patiekalai[i].gaukKaina();
			}
			for (int i = 0; i < (int)produktai.size(); i++){
				kaina += produktai[i].gaukKaina();
			}
			kaina *= porcijuKiekis;
		}
	}
};

/*prototypes*/
produktas& sukurkProdukta(std::string, float, float);
patiekalas& sukurkPatiekala(std::string);
void skaitykInformacija(std::string);
void skaitykEilute(std::ifstream&);
std::string gaukFailoPavadinima();
void gaukFailusAplanke(std::vector<std::string>*);
bool fileTypeIsSupported(std::string);
void spausdinkPavadinimus(std::vector<std::string>*);
void ClearScreen();
void processInfo(std::vector<std::string>);
float toFloat(std::string);
float useInfoAsValue(std::vector<std::string>, int);
void getValue(float&, std::vector<std::string>, int&);
void getValues(std::vector<float>*, std::vector<std::string>, int);
bool itIsAProduct(std::string, produktas*&);
bool itIsADish(std::string, patiekalas*&);
void sukurkPatiekala(patiekalas&, std::vector<std::string>, int);
float kiekKainuosVienaDiena();
float average(std::vector<patiekalas>);
void openResultsFile();
std::string toString(int);
bool fileExists(const std::string&);
void skaiciuokSanaudas();
void skaiciuokVidutineKaina();

std::vector<produktas> produktai,
					   sanaudos;
std::vector<patiekalas> patiekalai,
						pilniValgiai[3];
int eilutesNumeris = 0;
std::string exePath;
const std::string suppTypesFile = "supportedFileTypes.cfg";
const float eur = 3.45280f,
			daysInAYear = 365.242199f;
float menesineKaina = 0.f;
bool men = false;
std::ofstream results;

int _tmain(int argc, _TCHAR* argv[]){
	openResultsFile();
	skaitykInformacija(gaukFailoPavadinima());
	skaiciuokSanaudas();
	skaiciuokVidutineKaina();
	results.close();
	return 0;
}

void skaiciuokVidutineKaina(){
	float kkvd = kiekKainuosVienaDiena();
	results
		<< "Taip maitinantis vidutiniska kaina uz :" << std::endl
		<< "		1 Diena  = " << kkvd << ";" << std::endl
		<< "		1 Menesi = " << kkvd*daysInAYear / 12 << ";" << std::endl
		<< "		1 Metus  = " << kkvd*daysInAYear << ";" << std::endl
		<< "		4 Metus  = " << kkvd*daysInAYear * 4 << ";";
}
void skaiciuokSanaudas(){
	for (int i = 0; i < 3; i++){
		for (int o = 0; o < (int)pilniValgiai[i].size(); o++){
			std::vector<produktas> temp = pilniValgiai[i][o].gautiSanaudas();
			for (int p = 0; p < (int)temp.size(); p++){
				bool found = false;
				for (int l = 0; l < (int)sanaudos.size(); l++){
					if (temp[p].pavadinimas == sanaudos[l].pavadinimas){
						sanaudos[l].nustatykNaudojamaPorcijuKiekis(sanaudos[l].gaukNaudojamaPorcijuKieki() + temp[p].gaukNaudojamaPorcijuKieki() / pilniValgiai[i].size());
						found = true;
						break;
					}
				}
				if (!found) { 
					sanaudos.push_back(temp[p]);
					sanaudos[sanaudos.size() - 1].nustatykNaudojamaPorcijuKiekis(sanaudos[sanaudos.size() - 1].gaukNaudojamaPorcijuKieki() / pilniValgiai[i].size());
				}
			}
		}
	}
	results << std::endl
		<< "Produktu pakuociu sanauda per :" << std::endl << std::fixed << std::setw(20) << "produktas || " << std::setw(8) << "diena |" << std::setw(9) << "menesi |" << std::setw(10) << "metus |" << std::setw(11) << "4_metus" << std::endl
		<< std::string(63, '-') << std::endl;
	for (int i = 0; i < (int)sanaudos.size(); i++){
		float
			diena = sanaudos[i].gaukNaudojamaPorcijuKieki() / sanaudos[i].porcijuKiekis,
			metus = diena * daysInAYear,
			menesi = metus / 12,
			metus4 = metus * 4;
		results << std::fixed << std::setw(20) << sanaudos[i].pavadinimas << std::setw(8) << std::setprecision(3) << diena << std::setw(9) << menesi << std::setw(10) << metus << std::setw(13) << metus4 << std::endl;
	}
	results << std::string(63, '-') << std::endl;
}
std::string toString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}
void openResultsFile(){
	int number = 0;
	std::string name;
	do{
		number++;
		name = "results #" + toString(number) + ".cfg";
	} while (fileExists(name));
	results.open(name);
}
bool fileExists(const std::string& fileName){
	std::ifstream f(fileName.c_str());
	if (f.good()) {
		f.close();
		return true;
	}
	else {
		f.close();
		return false;
	}
}
float average(std::vector<patiekalas> valgis){
	float suma = 0.f;
	for (patiekalas& n : valgis) suma += n.gaukKaina();
	return (suma / (float)valgis.size());
}
float kiekKainuosVienaDiena(){
	return (average(pilniValgiai[0]) + average(pilniValgiai[1]) + average(pilniValgiai[2])+menesineKaina*12/daysInAYear);
}
float toFloat(std::string _string){
	return std::stof(_string);
}
produktas& sukurkProdukta(std::string pavadinimas, float produktoNevienetineKaina, float porcijuKiekis){
	produktas _produktas;
	_produktas.pavadinimas = pavadinimas;
	_produktas.nustatykPorcijuKieki(porcijuKiekis);
	_produktas.nustatykProduktoNevienetineKaina(produktoNevienetineKaina);
	if (men) {
		menesineKaina += produktoNevienetineKaina;
		_produktas.menesineKaina = true;
		men = false;
	}
	produktai.push_back(_produktas);
	return produktai[produktai.size() - 1];
}
patiekalas& sukurkPatiekala(std::string pavadinimas){
	patiekalas _patiekalas;
	_patiekalas.pavadinimas = pavadinimas;
	patiekalai.push_back(_patiekalas);
	return patiekalai[patiekalai.size() - 1];
}
void skaitykInformacija(std::string failoPavadinimas){
	std::ifstream failas;
	failas.open(exePath + failoPavadinimas);
	if (failas.is_open()){
		results << std::fixed << std::left << "Type " << std::setw(18) << "Name" << std::setw(14) << std::right << "Price" << std::endl << std::string(38, '-') << std::endl;
		while (!failas.eof()&&eilutesNumeris<256){
			skaitykEilute(failas);
		}
		failas.close();
		results << std::string(38, '-') << std::endl;
	}
	else results << "failed to open file " << failoPavadinimas << std::endl;
}
void skaitykEilute(std::ifstream &failas){
	eilutesNumeris++;
	std::string eilute;
	std::getline(failas, eilute);
	std::vector<std::string> info;
	std::istringstream iss(eilute);
	while (iss){
		std::string sub;
		iss >> sub;
		info.push_back(sub);
		if (sub.substr(0, 2) == "//") break;
	}
	if (info.size() != 0) info.pop_back();
	int w=5; for (int i = 0; i < (int)info.size(); i++){
		switch (i){
		case 1:
			w = 20;
			break;
		default:
			w = 5; 
			break;
		}
	}
	processInfo(info);
}
std::string gaukFailoPavadinima(){
	std::string failoPavadinimas="";
	std::vector<std::string> pavadinimai;
	gaukFailusAplanke(&pavadinimai);
	int pavadinimoNumeris=-1;
	while (pavadinimoNumeris<1 || pavadinimoNumeris>(int)pavadinimai.size()){
		spausdinkPavadinimus(&pavadinimai);
		std::cout << "Iveskite norimo failo numeri : ";
		std::cin >> pavadinimoNumeris;
		if (std::cin.fail()){
			std::cin.clear();
			std::cin.ignore();
			pavadinimoNumeris = -1;
		}
	}
	ClearScreen();
	return pavadinimai[pavadinimoNumeris-1];
}
void gaukFailusAplanke(std::vector<std::string>* pavadinimuLaikykla){
	char ownPth[MAX_PATH];
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL){
		GetModuleFileNameA(hModule, ownPth, (sizeof(ownPth)));
		char
			drive[_MAX_DRIVE],
			fname[_MAX_FNAME],
			ext[_MAX_EXT];
		_splitpath_s(ownPth, drive, ownPth, fname, ext);
		exePath = (std::string)drive + ownPth;
		std::string path = exePath + "*.*";
		HANDLE hFind;
		WIN32_FIND_DATAA FindFileData;

		if ((hFind = FindFirstFileA(path.c_str(), &FindFileData)) != INVALID_HANDLE_VALUE){
			do{
				std::string _string;
				_string = FindFileData.cFileName;
				if (_string != "."&&_string != ".."&&fileTypeIsSupported(_string)){
					pavadinimuLaikykla->push_back(_string);
				}
			} while (FindNextFileA(hFind, &FindFileData));
			FindClose(hFind);
		}
	}
}
bool fileTypeIsSupported(std::string filename){
	bool ret = false;
	std::ifstream supportedFiletypes;
	std::string filePath = exePath + suppTypesFile;
	supportedFiletypes.open(filePath);
	if (!supportedFiletypes.is_open()){
		std::ofstream newFile;
		newFile.open(filePath);
		newFile << 
			"//To add a new supported filetype simply create a new line and write the filetype there (example: txt). Keep in mind that some filetypes cannot be read and therefor will cause the program to misbehave or crash\n"
			"txt\n";
		newFile.close();
		supportedFiletypes.open(filePath);
	}
	std::string
		line,
		ext = filename.substr(filename.find_last_of(".") + 1);
	while (!supportedFiletypes.eof() && !ret){
		std::getline(supportedFiletypes, line);
		if (line.substr(0, 2) != "//"){
			if (ext == line) ret = true;
		}
	}
	supportedFiletypes.close();
	return ret;
}
void spausdinkPavadinimus(std::vector<std::string>* pavadinimai){
	ClearScreen();
	for (int i = 0; i<(int)pavadinimai->size(); i++){
		std::cout << "#" << i+1 << " - " << (*pavadinimai)[i] << std::endl;
	}
}
void ClearScreen(){
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}
void processInfo(std::vector<std::string> info){
	int size = (int)info.size();
	if (size >= 1){
		results << info[0] << " ";
		if (info[0] == "Pr" && size >= 4){
			std::vector<float> value = {0.f, 0.f};
			getValues(&value, info, 2);
			produktas& prod=sukurkProdukta(info[1],value[0],value[1]);
			results << std::fixed << std::setw(20) << std::left << info[1] << std::setw(7) << std::setprecision(5) << " " << prod.gaukKaina();
		}
		else if ((info[0] == "Pu" || info[0] == "Pi" || info[0] == "Va" || info[0] == "Pt") && size >= 2){
			patiekalas* pat;
			int location, which;
			if (info[0] == "Pt") {
				pat = &sukurkPatiekala(info[1]);
				location = 2;
			}
			else{
				which = 0;
				if (info[0] == "Pi") which = 1;
				else if (info[0] == "Va") which = 2;
				patiekalas temp;
				pilniValgiai[which].push_back(temp);
				pat = &pilniValgiai[which][pilniValgiai[which].size() - 1];
				location = 1;
			}
			sukurkPatiekala(*pat, info, location);
			if (location == 2) results << std::fixed << std::setw(20) << info[1] << std::setw(7) << std::setprecision(5) << " " << pat->gaukKaina();
			else results << "Maistas #" << std::fixed << std::setw(11) << pilniValgiai[which].size() << std::setw(7) << std::setprecision(5) << " " << pat->gaukKaina();
		}
		results << std::endl;
	}
}
float useInfoAsValue(std::vector<std::string> info, int location){
	if (info[location] == "E") return eur;
	else if (info[location] == "men") { men = true; return 0; }
	else return toFloat(info[location]);
}
void getValue(float& value, std::vector<std::string> info, int& location){
	value = useInfoAsValue(info, location);						location++;
	if (location!=(int)info.size() && info[location] == "*"){	location++;
		value *= useInfoAsValue(info, location);				location++;
	}
}
void getValues(std::vector<float>* value, std::vector<std::string> info, int location){
	for (int i = 0; i < (int)value->size(); i++){
		getValue((*value)[i], info, location);
	}
}
bool itIsAProduct(std::string info, produktas*& prop){
	int size = (int)produktai.size();
	for (int i = 0; i < size; i++){
		if (info == produktai[i].pavadinimas){
			prop = &produktai[i];
			return true;
		}
	}
	return false;
}
bool itIsADish(std::string info, patiekalas*& patp){
	int size = (int)patiekalai.size();
	for (int i = 0; i < size; i++){
		if (info == patiekalai[i].pavadinimas){
			patp = &patiekalai[i];
			return true;
		}
	}
	return false;
}
void sukurkPatiekala(patiekalas& pat, std::vector<std::string> info, int location){
	int size = (int)info.size();
	float value;
	patiekalas* patp = nullptr;
	produktas* prop = nullptr;
	std::string last;
	do{
		value = 1.f;
		if (!itIsAProduct(info[location], prop) && !itIsADish(info[location], patp)){
			std::cout << "Nera nei produkto, nei patiekalo, pavadinimu " << info[location] << ", kuris butu sukurtas pries " << info[1] << "." << std::endl;
		}
		else{
			location++;
			if (location < size && info[location] == "*"){
				location++;
				if (location < size){
					value = toFloat(info[location]);			location++;
				}
				else std::cout << "Ties " << eilutesNumeris << " eilute gale paliktas \"*\" zenklas.";
			}
			if (patp == nullptr) { pat.pridekProdukta(*prop, value); prop = nullptr; }
			else { pat.pridekPatiekala(*patp, value); patp = nullptr; }
		}
		if (location < size){
			last = info[location]; location++;
		}
		else last = "";
	} while (last == ",");
}