#pragma once

#include <string>
#include <ctime>
#include <climits>
#include <vector>
#include <algorithm> // для std::sort
#include <functional> // для std::greater


enum class Combination
{
	ONE = 1,
	TWO = 2,
	THREE = 3,
	FOUR = 4,
	FIVE = 5,
	SIX = 6,
	PAIR,              // Пара (два одинаковых)
	TWO_PAIRS,         // Две пары
	THREE_OF_A_KIND,   // Тройка
	FULL_HOUSE,        // Фулл-хаус (пара + тройка)
	STREET_SMALL,      // Стрит (1-2-3-4-5)
	STREET_BIG,		   // Стрит (2-3-4-5-6)
	FOUR_OF_A_KIND,    // Каре (четыре одинаковых)
	FIVE_OF_A_KIND,    // Покер (пять одинаковых)
	SUM_OF_A_KIND
};

class IBoardGameObserver
{
public:
	virtual void UpdatePrizeRow(int row) = 0;
	virtual void UpdatePrizeColumn(int column) = 0;
};

struct BoardGame: public IBoardGameObserver
{
	static const int N_ROW = 16;
	static const int N_COLUMN = 6;
	const int PENALTY = 100;
	int sumScool = 0;
	int sumTotal = 0;
	int arr[N_ROW][N_COLUMN] = {};

	void SetSumScool(int val) {
		if (val == INT_MAX) return;
		sumScool += val;
		sumTotal += val;
	}

	void SetTotalSum(int val) {
		if (val == INT_MAX) return;
		sumTotal += val;
	}

	bool SetValue(int val, Combination symb) {

		if (val == 0) val = INT_MAX;
		int row = static_cast<int>(symb) - 1;
		if (Combination::ONE <= symb && symb <= Combination::SIX) {			
			if (val < 0 && -1 * val > sumScool) {
				bool isFillSymbol = IsFillSymbolBoard();
				if (!isFillSymbol)	return false;
				val *= PENALTY; //  умножаем на величину штрафа
			}
			return SetScoolCeil(val, row);
		}
		return SetCeil(val, row);
	}

	bool SetScoolCeil(int val, int row )
	{
		if (row < 0 || row > 6) return false;
		int i = 0;
		while (arr[row][i] && i < N_COLUMN - 1) i++;
		if (i == N_COLUMN - 1) return false;
		arr[row][i] = val;
		SetSumScool(val);
		if (val < 0 && arr[row][N_COLUMN - 1] == 0)
			SetRowPrize(row, INT_MAX);
		if (i == N_COLUMN - 2 ) {
			if (arr[row][N_COLUMN - 1] == 0) {
				int sum = GetSumPrizRowScool(row);
				if (sum == 0) sum = row + 1;
				SetRowPrize(row, sum);
				SetTotalSum(sum);
			}
			if (boardGameObserver) boardGameObserver->UpdatePrizeRow(row);
		}
		int max = GetSumPrizeColumn(i);
		if (max && arr[N_ROW - 1][i] == 0) {
			SetColumnPrize(i, max);
			SetTotalSum(max);
		}
		if (max && boardGameObserver) boardGameObserver->UpdatePrizeColumn(i);
		return true;
	}

	int GetMaxForRow(int r) {
		int max = arr[r][0];
		for (int i = 0; i < N_COLUMN - 1; ++i)
			if (max < arr[r][i]) max = arr[r][i];
		return max;
	}

	int GetSumPrizeColumn(int col)
	{
		int max = 0;
		int i = 0;
		for (; i < 6 ; ++i) {
			if (arr[i][col] == 0) break;
			if (max < arr[i][col] && arr[i][col] != INT_MAX)  max = arr[i][col];
		}
		if (i < 6) return 0;
		for (; i < N_ROW - 1; ++i) {
			if (arr[i][col] == 0) break;
			if (max < arr[i][col] && arr[i][col] != INT_MAX) max = arr[i][col];
		}
		if (i < N_ROW - 1) return 0;
		return max;
	}

	int GetSumPrizRowScool(int row) {
		int sum = 0;
		for (int i = 0; i < N_COLUMN - 1; ++i)
			if (arr[row][i] != INT_MAX) sum += arr[row][i];
		return sum;
	}


	bool SetCeil( int val, int row)
	{
		int i = 0;
		while (arr[row][i] && i < N_COLUMN - 1) i++;
		if (i == N_COLUMN - 1) return false;
		arr[row][i] = val;
		SetTotalSum(val);
		if (val == INT_MAX) {
			SetRowPrize(row, INT_MAX);
			SetColumnPrize(i, INT_MAX);
		}
		if (i == N_COLUMN - 2 ) {
			if (arr[row][N_COLUMN - 1] == 0) {
				int max = GetMaxForRow(row);
				SetRowPrize(row, max);
				SetTotalSum(max);
			}
			if (boardGameObserver) boardGameObserver->UpdatePrizeRow(row);
		}
		// заполнить приз по вертикали
		int max = GetSumPrizeColumn(i);
		if (max && arr[N_ROW - 1][i] == 0) {
			SetColumnPrize(i, max);
			SetTotalSum(max);			
		}
		if (max && boardGameObserver) boardGameObserver->UpdatePrizeColumn(i);
		return true;
	}

	// Проверяет заполнение всего игрового поля
	bool IsFillSymbolBoard() {
		for (int i = 6; i < N_ROW - 1; ++i)
			if (arr[i][N_COLUMN - 2] == 0) return false;
		return true;
	}

	bool IsFillBoard() {
		for (int i = 0; i < N_ROW - 1; ++i)
			if (arr[i][N_COLUMN - 2] == 0) return false;
		return true;
	}

	void SetColumnPrize(int col, int val) {
		arr[N_ROW - 1][col] = val;
	}
	void SetRowPrize(int row, int val) {
		arr[row][N_COLUMN - 1] = val;
	}

	void UpdatePrizeRow(int row) {
		if (arr[row][N_COLUMN - 1] == 0)
			SetRowPrize(row, INT_MAX);
	}

	void UpdatePrizeColumn(int col) {
		if (arr[N_ROW - 1][col] == 0)
			SetColumnPrize(col, INT_MAX);
	}

	void SetBoardGameObserver(IBoardGameObserver* boardGameObserver) {
		this->boardGameObserver = boardGameObserver;
	}

	private:
		IBoardGameObserver* boardGameObserver = nullptr;
};

class Payer
{
public:

	std::wstring name;
	BoardGame boardGame;
	
	Payer(std::wstring _name):name(_name) {

	}

	bool SetValue(int val, Combination symb) {
		return boardGame.SetValue(val, symb);
	}

	bool IsAllFillField() {	
		return boardGame.IsFillBoard();
	}

};

class Game
{
public :
	std::vector<Payer> vPaers;
	static const int N_STEP = 3;
	int currStep = 0;
	Game(int nPlaers, std::vector<std::wstring> names) {
		for (auto &item : names) {
			vPaers.emplace_back(Payer(item));
		}
		vPaers[0].boardGame.SetBoardGameObserver(&vPaers[1].boardGame);
		vPaers[1].boardGame.SetBoardGameObserver(&vPaers[0].boardGame);
	}

	Payer& GetCurrentPayer() {
		return vPaers[index];
	}

	bool NextPayer() 
	{
		index++;
		index %= vPaers.size();
		currStep = 0;
		return true;
	}

	bool SetValue(int val, Combination symb) {
		if (currStep == 1) val *= 2; // удваиваем , если с первого раза выпала комбинация
		return vPaers[index].SetValue(val, symb);
	}

	bool IsGameOver() {
		for (auto& item : vPaers) {
			if (!item.IsAllFillField()) return false;
		}
		return true;
	}

private:	
	int index = 0;
};

class ArrayCube 
{
public:
	static const int N_COUNT_DICE = 5;

	std::vector<int> diceValues;
	std::vector<int> stateUp;
	int highlightedDice;

	ArrayCube() :highlightedDice(0)
	{
		srand(time(0));
		diceValues.resize(N_COUNT_DICE, 1);
		stateUp.resize(N_COUNT_DICE, 0);
		srand(time(0));
	}

	void SetDiceRandom()
	{
		for (int i = 0; i < N_COUNT_DICE; ++i) {
			if (stateUp[i] == 0)
				diceValues[i] = rollDice();
		}
	}

	int NextSelect() {
		highlightedDice = (highlightedDice + 1) % N_COUNT_DICE;
		return highlightedDice;
	}

	int PrevSelect() {
		highlightedDice = (highlightedDice - 1 + N_COUNT_DICE) % N_COUNT_DICE;
		return highlightedDice;
	}

	void SetUp() {
		stateUp[highlightedDice] = 1;
	}

	void SetDown() {
		stateUp[highlightedDice] = 0;
	}

	void Reset() {
		for (int i = 0; i < stateUp.size(); ++i) {
			stateUp[i] = 0;
			diceValues[i] = i + 1;
		}
		highlightedDice = 0;
	}

private:

	int rollDice() {
		return rand() % 6 + 1;
	}

};

class ScoreCalculator {
public:
	virtual ~ScoreCalculator() = default;
	virtual int calculate( std::vector<int>& dice) const = 0;
	Combination symbol;
};

class ScoolCalculator : public ScoreCalculator
{
public:
	ScoolCalculator()
	{
		symbol = Combination::ONE;
	}
	void SetKey(int key) 
	{
		symbol = static_cast<Combination>(key - '0');
	}
	int calculate( std::vector<int>& dice) const override
	{
		int keyDice = static_cast<int>(symbol);
		int sm = 0;
		for (int i = 0; i < dice.size(); ++i) 
			if (dice[i] == keyDice) sm++;
		return (sm - 3) * keyDice;
	}
};

class PairCalculator : public ScoreCalculator 
{
public:
	PairCalculator() 
	{
		symbol = Combination::PAIR;
	}
	int calculate( std::vector<int>& dice) const override
	{
		// Сортировка в порядке убывания
		std::sort(dice.begin(), dice.end(), std::greater<int>());
		for (int i = 0; i < dice.size() - 1; ++i) {
			if (dice[i] == dice[i + 1]) {
				return 2 * dice[i];
			}
		}
		return 0;
	}
};

class TwoPairCalculator : public ScoreCalculator
{
public:
	TwoPairCalculator()
	{
		symbol = Combination::TWO_PAIRS;
	}
	int calculate(std::vector<int>& dice) const override
	{
		// Сортировка в порядке убывания
		std::sort(dice.begin(), dice.end(), std::greater<int>());
		int sum = 0;
		int i = 0;
		while( i < dice.size() - 1 ) {
			if (dice[i] == dice[i + 1]) {
				sum += 2 * dice[i];
				i++;
			}
			i++;
		}
		return sum;
	}
};

class FullHouseCalculator : public ScoreCalculator
{
public:
	FullHouseCalculator()
	{
		symbol = Combination::FULL_HOUSE;
	}
	int calculate(std::vector<int>& dice) const override
	{
		if( dice.size() < 5 ) return 0;
		// Сортировка в порядке убывания
		std::sort(dice.begin(), dice.end(), std::greater<int>());
		if (dice[0] == dice[1] && dice[2] == dice[4])
			return 2 * dice[0] + 3 * dice[2];
		if (dice[0] == dice[2] && dice[3] == dice[4] )
			return 3 * dice[0] + 2 * dice[3];
		return 0;
	}
};

class StreetSmallCalculator : public ScoreCalculator
{
public:
	StreetSmallCalculator()
	{
		symbol = Combination::STREET_SMALL;
	}
	int calculate(std::vector<int>& dice) const override
	{
		// Сортировка в порядке возрастания
		std::sort(dice.begin(), dice.end());
		int sum = 0;
		for (int i = 0; i < dice.size(); ++i) {
			if (dice[i] != i + 1) return 0;
			sum += dice[i];
		}
		return sum;
	}
};

class StreetBigCalculator : public ScoreCalculator
{
public:
	StreetBigCalculator()
	{
		symbol = Combination::STREET_BIG;
	}
	int calculate(std::vector<int>& dice) const override
	{
		// Сортировка в порядке возрастания
		std::sort(dice.begin(), dice.end());
		int sum = 0;
		for (int i = 0; i < dice.size(); ++i) {
			if (dice[i] != i + 2) return 0;
			sum += dice[i];
		}
		return sum;
	}
};

class ThreeOfAKindCalculator : public ScoreCalculator 
{ 
public:
	ThreeOfAKindCalculator()
	{
		symbol = Combination::THREE_OF_A_KIND;
	}
	int calculate( std::vector<int>& dice) const override
	{
		// Сортировка в порядке убывания
		std::sort(dice.begin(), dice.end(), std::greater<int>());
		for (int i = 0; i < dice.size() - 2; ++i) {
			if (dice[i] == dice[i + 1] && dice[i] == dice[i + 2]) {
				return 3 * dice[i];
			}
		}
		return 0;
	}

};

class FourOfAKindCalculator : public ScoreCalculator
{
public:
	FourOfAKindCalculator()
	{
		symbol = Combination::FOUR_OF_A_KIND;
	}
	int calculate(std::vector<int>& dice) const override
	{
		const int N_FOUR_ADD = 20;
		// Сортировка в порядке убывания
		std::sort(dice.begin(), dice.end(), std::greater<int>());	
		int n = dice.size();		
		if( dice[0] == dice[n - 2] ) 
			return (n - 1) * dice[0] + N_FOUR_ADD;
		if (dice[1] == dice[n - 1])
			return (n - 1) * dice[1] + N_FOUR_ADD;
		return 0;
	}

};

class FiveOfAKindCalculator : public ScoreCalculator
{
public:
	FiveOfAKindCalculator()
	{
		symbol = Combination::FIVE_OF_A_KIND;
	}
	int calculate(std::vector<int>& dice) const override
	{
		const int N_FIVE_ADD = 50;
		// Сортировка в порядке убывания
		std::sort(dice.begin(), dice.end(), std::greater<int>());
		int n = dice.size();
		return dice[0] == dice[n - 1] ? (n) * dice[0] + N_FIVE_ADD : 0;
	}

};

class SumKindCalculator : public ScoreCalculator
{
public:
	SumKindCalculator()
	{
		symbol = Combination::SUM_OF_A_KIND;
	}
	int calculate(std::vector<int>& dice) const override
	{
		int sum = 0;
		for (int i = 0; i < dice.size(); ++i) 
			sum += dice[i];
		return sum;
	}

};

