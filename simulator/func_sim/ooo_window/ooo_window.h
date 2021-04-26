#ifndef OOO_WINDOW_H
#define OOO_WINDOW_H

#include <func_sim/operation.h>

#include <iostream>
#include <list>

template <typename FuncInstr> 
class OOOWindow
{
	size_t max_instr;
	uint32 cycles;
	uint32 number_independ_instr;
	std::list<FuncInstr> window;

	uint32 independent_in_a_cycle();

public:
	void write_instruction(FuncInstr instruction);
	double get_avg_independent_instructions();

	OOOWindow(std::size_t size);
	~OOOWindow();
};

template <typename FuncInstr> 
OOOWindow<FuncInstr>::OOOWindow(std::size_t size) 
															:	max_instr(size)
															, cycles(0)
															, number_independ_instr(0)
{}

template <typename FuncInstr> 
OOOWindow<FuncInstr>::~OOOWindow()
{
	std::cout << get_avg_independent_instructions() << std::endl;
}

template <typename FuncInstr> 
void OOOWindow<FuncInstr>::write_instruction(FuncInstr instruction)
{
	if (max_instr != 0)
	{
		if (window.size() != max_instr)
		{
			window.push_front(instruction);
		}
		else
		{
			window.pop_back();
			window.push_front(instruction);
		}

		//Checking only when the window is full
		if (window.size() == max_instr)
		{
			cycles++;
			number_independ_instr += independent_in_a_cycle();
		}
	}
}

template <typename FuncInstr>
double OOOWindow<FuncInstr>::get_avg_independent_instructions()
{
	if (cycles != 0)
		return static_cast<double>(number_independ_instr) / static_cast<double>(cycles);
	else
		return 0.0;
}

template <typename FuncInstr>
uint32 OOOWindow<FuncInstr>::independent_in_a_cycle()
{
	bool independent = true;
	uint32 num_independ_instr_in_cycle = 1; // the last instr. in the window is always independent
	for (typename std::list<FuncInstr>::iterator it=window.begin(); it != std::prev(window.end()); ++it)
	{
		independent = true;
		auto src_1 = it -> get_src_num(0);
		auto src_2 = it -> get_src_num(1);
		auto src_3 = it -> get_src_num(2);
	
		for (typename std::list<FuncInstr>::iterator jt=std::next(it); jt != window.end(); ++jt)
		{
			auto dest = jt -> get_dst_num();
			if ((src_1 == dest || src_2 == dest || src_3 == dest) 
																					&& !(src_1.is_zero() && dest.is_zero())
																					&& !(src_2.is_zero() && dest.is_zero())
																					&& !(src_3.is_zero() && dest.is_zero()))
				independent = false;
		}

		if (independent)
			num_independ_instr_in_cycle++;
	}
	return num_independ_instr_in_cycle;
}

#endif