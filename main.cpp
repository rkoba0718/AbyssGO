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
std::pair<int, std::vector<int>> calculate_charge_stones(int);
int calculate_get_stones(std::vector<int>);

/*
	課金にかかる金額と得られる聖晶石の個数の組み
*/
const std::vector<CHARGES_LIST> charges_list = {
	{480, 5}, {1600, 21}, {3000, 41}, {4900, 77}, {10000, 168}};

/*
	main関数
	所持したい聖晶石の個数、現在保有している聖晶石の個数を入力すると、必要な聖晶石の購入に最適な購入パターンを出力する。
*/
int main()
{
	int before_stones;
	int after_stones;

	after_stones = input("課金後に所持したい聖晶石の個数を入力してください: ");
	before_stones = input("現在保有している聖晶石の個数を入力してください: ");

	auto result = calculate_charge_stones(after_stones - before_stones);
	int stones = calculate_get_stones(result.second);
	std::cout << "最低の値段: " << result.first << "円\n";
	std::cout << "得られる聖晶石の個数: " << stones << "個\n";
	std::cout << "購入パターン:\n";
	for (int i = 0; i < LIST_SIZE; ++i)
	{
		std::cout << charges_list[i].price << "円: " << result.second[i] << "回\n";
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
	引数：int
	戻り値：std::pair<int, std::vector<int>>
	与えられた引数を購入するために、動的計画法により最適な購入方法を計算する関数。
	欲しい聖晶石の個数以上の最低金額の購入方法を求め、その時の購入金額と購入パターンの組みを返す。
*/
std::pair<int, std::vector<int>> calculate_charge_stones(int required_stones)
{
	std::vector<std::vector<int>> dp_table(LIST_SIZE, std::vector<int>(required_stones + 1, INFINITY));
	dp_table[0][0] = 0;
	for (int i = 1; i <= required_stones; i++)
	{
		dp_table[0][i] = ((i - 1) / charges_list[0].num + 1) * charges_list[0].price;
	}

	for (int j = 1; j < LIST_SIZE; j++)
	{
		for (int i = 1; i <= required_stones; i++)
		{
			dp_table[j][i] = dp_table[j - 1][i];
			int num_tb_j = 1;
			int rem;
			do
			{
				rem = i - num_tb_j * charges_list[j].num;
				int tmp_price = dp_table[j - 1][rem < 0 ? 0 : rem] + num_tb_j * charges_list[j].price;
				if (tmp_price < dp_table[j][i])
				{
					dp_table[j][i] = tmp_price;
				}
				num_tb_j++;
			} while (rem > 0);
		}
	}

	int minumum_charges = dp_table[LIST_SIZE - 1][required_stones];
	std::vector<int> charges_patterns(LIST_SIZE, 0);

	for (int i = LIST_SIZE - 1; i >= 0; i--)
	{
		if (minumum_charges <= 0)
		{
			break;
		}
		charges_patterns[i] = minumum_charges / charges_list[i].price;
		minumum_charges -= charges_patterns[i] * charges_list[i].price;
	}

	return {dp_table[LIST_SIZE - 1][required_stones], charges_patterns};
}

/*
	calculate_get_stones関数
	引数：std::vector<int>
	戻り値：int
	得られる聖晶石の個数を計算する関数。
*/
int calculate_get_stones(std::vector<int> charges)
{
	int stones = 0;

	for (int i = 0; i < LIST_SIZE; i++)
	{
		stones += charges[i] * charges_list[i].num;
	}

	return stones;
}