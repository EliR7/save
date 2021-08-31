#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <pqxx/pqxx>

const double z_threshold = 3.0;
const int rule2_threshold = 5;
const std::string rule1_header("Name\tAccount Number\tTransaction Number\tMerchant\tTransaction Amount\n");
const std::string rule2_header("Name\tAccount Number\tTransaction Number\tExpected Transaction Location\tActual Transaction Location\n");

pqxx::result rule(pqxx::connection& conn, const std::string& query) {
    pqxx::work txn{conn};
    pqxx::result r{txn.exec(query)};
    txn.commit();
    return r;
}

void report(const pqxx::result& r, const std::string& header, const std::string& rule) {
    std::ofstream rule_report(rule +"_report.txt");
    std::cout << header;
    rule_report << header;
    if (rule == "rule1") {
        for (auto row: r) {
            std::cout << row[0] << '\t'<< row[1].as<int>() << '\t' << row[2].as<int>() << '\t' 
                  << row[3] << '\t' << row[4].as<double>() << "\n";
            rule_report << row[0] << '\t'<< row[1].as<int>() << '\t' << row[2].as<int>() << '\t'
                     << row[3] << '\t' << row[4].as<double>() << "\n";
        }
    } 
    if (rule == "rule2") {
        for (auto row: r) {
            std::cout << row[0] << '\t'<< row[1].as<int>() << '\t' << row[2].as<int>() << '\t' 
                  << row[3] << '\t' << row[4] << "\n";
            rule_report << row[0] << '\t'<< row[1].as<int>() << '\t' << row[2].as<int>() << '\t'
                     << row[3] << '\t' << row[4] << "\n";
        }
    }
    rule_report.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
         std::cerr << "Usage: " << argv[0] << " <connection string> <z threshold>\n";
         return 1;
     }
     double z = z_threshold;
     if (argc > 2) {
         z = fabs(std::stod(argv[2]));
     }
     int n = rule2_threshold;
     if (argc > 3) {
         n = abs(std::stoi(argv[3]));
     }

     std::stringstream ss;
     ss << 
         "WITH stats AS (SELECT account_number,AVG(transaction_amount) mean,STDDEV(transaction_amount) sigma "
         "FROM save.transactions GROUP BY account_number), zscore AS (SELECT account_number,transaction_number,"
         "merchant_description,transaction_amount,abs(transaction_amount - stats.mean)/stats.sigma z "
         "FROM save.transactions INNER JOIN stats USING(account_number) WHERE stats.sigma IS NOT NULL AND stats.sigma > 0.0) "
         "SELECT first_name || ' ' || last_name AS \"Name\", account_number AS \"Account Number\","
         "transaction_number AS \"Transaction Number\", merchant_description AS \"Merchant\", transaction_amount "
         "AS \"Transaction Amount\" FROM zscore INNER JOIN save.account_info USING(account_number) WHERE z > " << z <<
         " ORDER BY \"Account Number\";";
     auto q1 = ss.str();

     ss.str(std::string());
     ss << 
         "WITH transaction_freq AS (select account_number,count(right(merchant_description,2)) transactions_in_state_count,"
         "right(merchant_description,2) state from save.transactions group by account_number, state),"
         "state_freq AS (select account_number,count(state) from transaction_freq group by account_number having "
         "count(state) > 1),total_freq AS (select transaction_freq.account_number,transaction_freq.transactions_in_state_count,"
         "transaction_freq.state from transaction_freq inner join state_freq using(account_number)),"
         "expected_location AS (select total_freq.account_number,total_freq.transactions_in_state_count,total_freq.state "
         "from total_freq where total_freq.transactions_in_state_count = ("
         "select max(a.transactions_in_state_count) expected from total_freq a where "
         "total_freq.account_number = a.account_number)),"
         "suspected_location AS (select total_freq.account_number,total_freq.state actual from total_freq "
         "where total_freq.transactions_in_state_count * " << n 
         << "<= (select transactions_in_state_count from expected_location "
         "where expected_location.account_number = total_freq.account_number)) "
         "select first_name || ' ' || last_name \"Name\", suspected_location.account_number \"Account Number\","
         "transaction_number \"Transaction Number\", expected_location.state \"Expected Transaction Location\","
         "suspected_location.actual \"Actual Transaction Location\" from suspected_location inner join expected_location "
         "using (account_number) inner join save.transactions using(account_number) inner join save.account_info "
         "using(account_number) where suspected_location.actual = right(merchant_description,2) order by account_number;";
     auto q2 = ss.str();

     pqxx::connection c{argv[1]};
     try {

         pqxx::result r1{rule(c, q1)};
         report(r1, rule1_header, "rule1");
            
         std::cout << "\n\n";

         pqxx::result r2{rule(c,q2)};
         report(r2, rule2_header, "rule2");
     }
     catch (pqxx::sql_error const &e)
     {
         std::cerr << "SQL error: " << e.what() << std::endl;
         std::cerr << "Query was: " << e.query() << std::endl;
         return 2;
     }
     catch (std::exception const &e)
     {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
     }
    //postgresql://save:secret@localhost/save
}
