#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cstdlib>
#include <vector>
#include<thread>
#include <conio.h>

using namespace std;


const string server = "localhost:3306";
const string username = "root";
const string password = "1";
sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::ResultSet* res;
sql::PreparedStatement* pstmt;
void db() {


	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}


	con->setSchema("vitamin_paradise");


};
class USR{
private:
public:
	int usrid;
	string usrname;
	string passwd;

	bool checkExist(string usrname, string passwd, bool reg) {
		con = driver->connect(server, username, password);
		con->setSchema("vitamin_paradise");
		std::auto_ptr<sql::Statement> stmt(con->createStatement());
		if (reg)
		{
			stmt->execute("SELECT * FROM users WHERE usrname = '" + usrname + "'");
			std::auto_ptr< sql::ResultSet > res;
			do
			{
				res.reset(stmt->getResultSet());
				while (res->next())
				{
					return true;
				}
			} while (stmt->getMoreResults());
			return false;
		}
		else
		{
			stmt->execute("SELECT * FROM users WHERE usrname = '" + usrname + "' AND passwd= '" + passwd + "'");
			std::auto_ptr< sql::ResultSet > res;
			do
			{
				res.reset(stmt->getResultSet());
				while (res->next())
				{
					return true;
				}
			} while (stmt->getMoreResults());
			return false;
		}
		// функция, чрез която се проверява всеки ред от таблицата на клиентите, за да види дали данните въведени от потребителя съществуват
	}
	
	void reg(string usrname, string passwd) {// въвежда данните на потребителя в таблицата
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		stmt->execute("INSERT INTO USERS(usr_id,usrname,passwd) VALUES(usr_id, '" + usrname + "', '" + passwd + "')");
	}
	
	

	int getusrid(string usrname) {// според името на потребителя, функцията връща неговия номер от дадената таблицата

		int usrid;
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		res = stmt->executeQuery("SELECT usr_id FROM users WHERE usrname = '" + usrname + "'");


		while (res->next()) {

			usrid = res->getInt("usr_id");
		}
		return usrid;
		}
};
class VIT {
public:
	string moreVits;// тази променлива съдържа стойност зададена от клиента, дали той има още нужди от витамини и ако няма да се прекъсне цикъла който го пита.
	vector<int> vitids;
	int currentvitid;
	bool checkvit(string vitname) {// проверява даи въведения витамин съществува
		std::auto_ptr<sql::Statement> stmt(con->createStatement());
		stmt->execute("SELECT * FROM vitamins WHERE vit_name = '" + vitname + "'");
		std::auto_ptr< sql::ResultSet > res;
		do
		{
			res.reset(stmt->getResultSet());
			while (res->next())
			{
				return true;
			}
		} while (stmt->getMoreResults());
		return false;
	}
	int getvid(string vitname) {//според името на витамина се връща неговия номер
		int vitid;
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		res = stmt->executeQuery("SELECT vit_id FROM vitamins WHERE vit_name = '" + vitname + "'");


		while (res->next()) {

			vitid = res->getInt("vit_id");
		}

		return vitid;
	};
	void insertvitotusr(int vitid, int usrid) {//попълва таблицата която е пълна с липсващите витамини с въведения витамин от потребителя и неговия номер 
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		pstmt = con->prepareStatement("INSERT INTO userstovitamins_needed(usr_id,vit_id) VALUES(?,?)");
		pstmt->setInt(1, usrid);
		pstmt->setInt(2, vitid);
		pstmt->execute();
	}
	vector<int> getvitids(int usrid) {//връща вектор който съдържа всички липсващи витамини за дадения потребител
		vector<int> results;
		string idusr = to_string(usrid);
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");

		res = stmt->executeQuery("SELECT vit_id FROM userstovitamins_needed WHERE usr_id = '" + idusr + "'");



		while (res->next()) {

			results.push_back(res->getInt("vit_id"));
		}

		return results;
	}
	bool existCheck(int usrid, int vitid) {// проверява дали въведения витамин вече е отбелязан като липсващ
		string usridstr = to_string(usrid);
		string vitidstr = to_string(vitid);
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		stmt->execute("SELECT * FROM userstovitamins_needed WHERE usr_id = '" + usridstr + "' AND vit_id= '" + vitidstr + "'");
		std::auto_ptr< sql::ResultSet > res;
		do
		{
			res.reset(stmt->getResultSet());
			while (res->next())
			{
				return true;
			}
		} while (stmt->getMoreResults());
		return false;
	}
};
class MENUS {
public:
	string changemenu;
	string mealcategories[4] = {"breakfast","lunch","snack","dinner"};
	int randmenuid;
	int mealid;
	vector<int> assignedMenus;
	vector<int> mealsformenu;
	vector<int> menuids;
	vector<int> getmenus(vector<int> vits) {// връща вектор, който съдържа всички подходящи менюта
		vector<int> results;
		string strvit;
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		for (int i = 0; i < vits.size(); i++)
		{
			strvit = "\0";
			strvit = to_string(vits[i]);

				res = stmt->executeQuery("SELECT mm.menu_id, COUNT(mm.menu_id) FROM mealstomenus mm JOIN meals m ON mm.m_id = m.m_id JOIN meal_details md ON m.m_id = md.m_id JOIN ingredientstovitamins itv ON md.itv_id = itv.itv_id WHERE vit_id = '" + strvit + "' GROUP BY mm.menu_id HAVING COUNT(mm.menu_id) >= 4");
				while (res->next()) {
					results.push_back(res->getInt("menu_id"));
				}
			

		};
		return results;
	}
	void assign(vector<int> menus, int usrid) {// попълва таблицата с назначените менюта и номера на потребителя
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		for (int i = 0; i < menus.size(); i++)
		{
			pstmt = con->prepareStatement("INSERT INTO menus_for_users(usr_id,menu_id) VALUES(?,?)");
			pstmt->setInt(1, usrid);
			pstmt->setInt(2, menus[i]);
			pstmt->execute();
		}

	}
	int getRandomMenu(int low, int high) {
		if (low > high) return high;
		return low + (std::rand() % (high - low + 1));
	}
	vector<int> getAssignedMenus(int usr_id) {//връща всички назначени дневни менюта в един вектор
		vector<int> result;
		string usrid_str = to_string(usr_id);
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		res = stmt->executeQuery("SELECT menu_id FROM menus_for_users WHERE usr_id = '" + usrid_str + "'");
		while (res->next())
		{
			result.push_back(res->getInt("menu_id"));
		}
		return result;
	}
	vector<int> getmeals(int menuid) {//връща всички ядения които са от определено меню в един вектор
		vector<int> result;
		string menustring = to_string(menuid);
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		res = stmt->executeQuery("SELECT m_id FROM mealstomenus WHERE menu_id = '" + menustring + "'");
		while (res->next())
		{
			result.push_back(res->getInt("m_id"));
		}
		return result;
	}
	string getspecmeal(string mealcat, vector<int> assignedMeals) {//връща номера на ядене и неговото име от определена категория, което е от определено меню;
		int mealid;
		string vecstring;
		string mealidstr;
		string mealname;
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		stmt = con->createStatement();
		stmt->execute("USE vitamin_paradise");
		for (int i = 0; i < assignedMeals.size(); i++)
		{
			vecstring = to_string(assignedMeals[i]);
			res = stmt->executeQuery("SELECT * FROM `meals` m JOIN meal_categories mc ON(m.mcat_id = mc.mcat_id) WHERE mc.mcat_name = '" + mealcat + "' and m.m_id = '" + vecstring + "' ");
			while (res->next())
			{
				mealid = (res->getInt("m_id"));
			}
		}
		mealidstr = to_string(mealid);
		res = stmt->executeQuery("SELECT m_name FROM meals WHERE m_id = '" + mealidstr + "'");
		while (res->next())
		{
			mealname = (res->getString("m_name"));
		}
		return mealname;
	}

	
};
int main()
{
	db();
	cout << "//////////////////VITAMIN PARADISE//////////////////////////" << endl;
	USR login;// обекта който се използва за да се логнем в нашия профил
	USR regist;// обекта, който използваме за да се регистрира потребителяи и  да се запишат кои витамини му липсват
	string vit; // променлива в която клиента въвежда витамина, който му липсва
	string cat;
	VIT vitamins;
	MENUS menu1;
	while (true)
	{
		cout << "Would you like to login or register" << endl;
		while (1)
		{
			cin >> cat;
			if (cat == "login" || cat == "register" || cat == "reg" || cat == "log" || cat == "r" || cat == "l")
			{
				break;

			}
			else
			{
				cout<<"Try again" << endl;
			}
		}
		
		if (cat == "login" || cat == "log" || cat == "l")
		{
			string usrnamecheck;
			string passwdcheck;
			while (true)
			{
				cout << "Please input your username: ";
				cin >> usrnamecheck;
				cout << "Please input your password: ";
				char ch;
				ch = _getch();
				while (ch != 13)
				{
					
					if (ch == 8)
					{
						if (passwdcheck.length() == 0)
						{
							ch = _getch();
							continue;
						}
						passwdcheck.erase(passwdcheck.length() - 1);
						cout << "\b \b";
						ch = _getch();
						continue;
					}
					passwdcheck.push_back(ch);
					cout << "*";
					ch = _getch();
				}
				if (login.checkExist(usrnamecheck, passwdcheck,false)) {
					break;
				}
				cout << "\nWrong credentials. PLease try again. " << endl;
			}
			
				cout << "\n////////////////////////////////// WELCOME " << usrnamecheck <<" //////////////////////////////////" << endl;
				login.usrname = usrnamecheck;
				login.usrid = login.getusrid(login.usrname);
				menu1.assignedMenus = menu1.getAssignedMenus(login.usrid);
				srand((unsigned int)time(0));
			while (true)
				{
					double random = (rand() / (double)RAND_MAX);
					menu1.randmenuid = menu1.assignedMenus[menu1.getRandomMenu(0, (menu1.assignedMenus.size()) - 1)];
					menu1.mealsformenu = menu1.getmeals(menu1.randmenuid);;
					cout << "YOUR PLAN FOR TODAY:" << endl;
					cout << "////////////////////" << endl;
					cout << "\nBreakfast: " << menu1.getspecmeal(menu1.mealcategories[0], menu1.mealsformenu) << endl;
					cout << "////////////////////" << endl;
					cout << "\nLunch: " << menu1.getspecmeal(menu1.mealcategories[1], menu1.mealsformenu) << endl;
					cout << "////////////////////" << endl;
					cout << "\nSnack: " << menu1.getspecmeal(menu1.mealcategories[2], menu1.mealsformenu) << endl;
					cout << "////////////////////" << endl;
					cout << "\nDinner: " << menu1.getspecmeal(menu1.mealcategories[3], menu1.mealsformenu) << endl;
					cout<< "////////////////////" << endl; 
					std::this_thread::sleep_for(2000ms);
					cout << "\n\nWould you like to try another menu?" << endl;
					cin >> menu1.changemenu;
					if (menu1.changemenu == "no" || menu1.changemenu == "n" || menu1.changemenu == "-")
					{
						cout << "Have a good day!" << endl;
						break;
					}
					
					
				}

		}
		else if (cat == "register" || cat == "reg" || cat == "r")
		{

			
			while (1)
			{
				string usrname;
				string passwd;
				char ch;
				cout << "Please input your username:";
				cin >> usrname;
				cout << "Please input your password:";
				ch = _getch();
				while (ch != 13)
				{
					if (ch == 8) 
					{
						if (passwd.length() == 0)
						{
							ch = _getch();
							continue;
						}
						passwd.erase(passwd.length() - 1);
						cout << "\b \b";
						ch = _getch();
						continue;
					}
					passwd.push_back(ch);
					cout << "*";
					ch = _getch();
				}
				
				if (regist.checkExist(usrname,passwd,true))
				{
					cout << "\nUsername already exists"<<endl;
					continue;
					
				}
				else
				{
					regist.usrname = usrname;
					regist.passwd = passwd;
					break;
				}
			}
			
			regist.reg(regist.usrname, regist.passwd);
			cout << "\nSuccesfully registered" << endl;
				regist.usrid = regist.getusrid(regist.usrname);
				while (1)				
				{
					vit ="\0";
					cout << "Which vitamin are you lacking?" << endl;
					cin >> vit;
					if (vitamins.checkvit(vit) && !vitamins.existCheck(regist.usrid, vitamins.getvid(vit)))
					{
						vitamins.currentvitid = vitamins.getvid(vit);
						vitamins.insertvitotusr(vitamins.currentvitid, regist.usrid);
						cout << "Thank you, we have taken that into note." << endl;

					}
					else
					{
						cout << "Whooops, something went  wrong. Please check if you are entering the correct vitamin or if you have entered a vitamin that you have already given." << endl;
						continue;
					}
					cout << "Are you lacking any other vitamins?" << endl;
					cin >> vitamins.moreVits;
					
					if (vitamins.moreVits == "no" || vitamins.moreVits == "n")
					{ 
						vitamins.vitids = vitamins.getvitids(regist.usrid);
						menu1.menuids = menu1.getmenus(vitamins.vitids);
						menu1.assign(menu1.menuids, regist.usrid);
						break;
					}
					
				}

		};
		if (cat == "login" || cat == "log" || cat == "l")
		{
			break;
		}
	};
	

	
	return 0;
}