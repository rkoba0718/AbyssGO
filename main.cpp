#include <iostream>
#include <vector>
#include <algorithm>

#define LIST_SIZE 5

struct CHARGES_LIST
{
	int price;
	int num;
};

/*
	関数のプロトタイプ宣言
*/
bool isNumber(const std::string &);
int input(const std::string &);
std::pair<int, std::vector<int>> calculate_charge_stones(int, std::vector<CHARGES_LIST>);
int calculate_get_stones(std::vector<int>, std::vector<CHARGES_LIST>);

/*
	課金にかかる金額と得られる聖晶石の個数の組み
	charges_list: {課金額, (有償石＋無償石)の個数}
	paid_charges_list: {課金額, 有償石の個数}
*/
const std::vector<CHARGES_LIST> charges_list = {
	{480, 5}, {1600, 21}, {3000, 41}, {4900, 77}, {10000, 168}};

const std::vector<CHARGES_LIST> paid_charges_list = {
	{480, 4}, {1600, 13}, {3000, 25}, {4900, 42}, {10000, 86}};

/*
	main関数
	所持したい聖晶石の個数、現在保有している聖晶石の個数を入力すると、必要な聖晶石の購入に最適な購入パターンを出力する。
*/
int main(int argc, char *argv[])
{
	std::string arg;
	std::vector<CHARGES_LIST> charges;
	int before_stones;
	int after_stones;

	if (argc > 1)
	{
		arg = argv[1];
		if (arg == "-p")
		{
			charges = paid_charges_list;
			after_stones = input("欲しい有償石の個数を入力してください: ");
			before_stones = input("現在所持している有償石の個数を入力してください: ");
		}
		else
		{
			std::cout << "コマンドエラー: " << arg << " は無効なコマンドです。\n";
			return 0;
		}
	}
	else
	{
		charges = charges_list;
		after_stones = input("課金後に所持したい聖晶石の個数を入力してください: ");
		before_stones = input("現在所持している聖晶石の個数を入力してください: ");
	}

	auto result = calculate_charge_stones(after_stones - before_stones, charges);
	int stones = calculate_get_stones(result.second, charges);

	std::cout << "******* 計算結果 *******\n";
	std::cout << "最低の値段: " << result.first << "円\n";
	if (arg == "-p")
	{
		std::cout << "得られる有償石の個数: " << stones << "個\n";
	}
	else
	{

		std::cout << "得られる聖晶石の個数: " << stones << "個\n";
	}
	std::cout << "購入パターン:\n";
	for (int i = 0; i < LIST_SIZE; ++i)
	{
		std::cout << charges[i].price << "円: " << result.second[i] << "回\n";
	}

	return 0;
}

/*
	isNumber関数
	引数：string
	戻り値：bool
	与えられた引数が数値であるかを判別する関数。
	数値ならばtrueを返し、そうでないならばfalseを返す。
*/
bool isNumber(const std::string &str)
{
	for (char const &c : str)
	{
		if (std::isdigit(c) == 0)
		{
			return false;
		}
	}
	return true;
}

/*
	input関数
	引数：string
	戻り値：int
	与えられた引数を標準出力に表示し、数字の入力を受け付ける関数。
	入力文字列をisNumber関数に渡し、数値かどうかを判定する。
	数値であった場合は、それを返し、そうでない場合はもう一度入力を受け付ける。
*/
int input(const std::string &str)
{
	std::string input;
	bool valid_input = false;

	do
	{
		std::cout << str;
		std::cin >> input;
		valid_input = isNumber(input);
		if (!valid_input)
		{
			std::cout << "入力エラー： 数字以外の文字列が含まれています。\n";
			std::cin.clear();
		}
	} while (!valid_input);

	return std::stoi(input);
}

/*
	calculate_charge_stones関数
	引数：int, std::vector<CHARGES_LIST>
	戻り値：std::pair<int, std::vector<int>>
	与えられた引数を購入するために、動的計画法により最適な購入方法を計算する関数。
	欲しい聖晶石の個数以上の最低金額の購入方法を求め、その時の購入金額と購入パターンの組みを返す。
*/
std::pair<int, std::vector<int>> calculate_charge_stones(int required_stones, std::vector<CHARGES_LIST> charges)
{
	int dp_size = required_stones;
	std::vector<std::vector<int>> dp(LIST_SIZE, std::vector<int>(dp_size + 1, INFINITY));
	dp[0][0] = 0;
	for (int i = 1; i <= dp_size; i++)
	{
		dp[0][i] = ((i - 1) / charges[0].num + 1) * charges[0].price;
	}

	for (int i = 1; i < LIST_SIZE; i++)
	{
		for (int j = 1; j <= dp_size; j++)
		{
			dp[i][j] = dp[i - 1][j];
			for (int k = 1; k <= j; k++)
			{
				int rem = j - k * charges[i].num;
				int tmp_price = dp[i - 1][rem < 0 ? 0 : rem] + k * charges[i].price;
				if (tmp_price < dp[i][j])
				{
					dp[i][j] = tmp_price;
				}
				if (rem < 0)
				{
					break;
				}
			}
		}
	}

	int minumum = dp[LIST_SIZE - 1][dp_size];
	std::vector<int> charges_patterns(LIST_SIZE, 0);
	int price = 0;

	for (int i = LIST_SIZE - 1; i >= 0; i--)
	{
		if (minumum <= 0)
		{
			break;
		}
		charges_patterns[i] = minumum / charges[i].price;
		minumum -= charges_patterns[i] * charges[i].price;
		price += charges_patterns[i] * charges[i].price;
	}

	return {price, charges_patterns};
}

/*
	calculate_get_stones関数
	引数：std::vector<int>, std::vector<CHARGES_LIST>
	戻り値：int
	得られる聖晶石の個数を計算する関数。
*/
int calculate_get_stones(std::vector<int> patterns, std::vector<CHARGES_LIST> charges)
{
	int stones = 0;

	for (int i = 0; i < LIST_SIZE; i++)
	{
		stones += patterns[i] * charges[i].num;
	}

	return stones;
}