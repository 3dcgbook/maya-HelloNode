#include <maya/MPxNode.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEvaluationNode.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

class HelloNode : public MPxNode
{
public:
	HelloNode();
	virtual ~HelloNode();
	static  void*       creator();
	
	static  MStatus     initialize();
	virtual MStatus     compute(const MPlug& plug, MDataBlock& data);

	virtual MStatus     setDependentsDirty(const MPlug& plug, MPlugArray& plugArray);
	virtual MStatus     preEvaluation(const  MDGContext& context, const MEvaluationNode& evaluationNode);

	static  MObject     input1; // 入力１のアトリビュート
	static  MObject     input2; // 入力２のアトリビュート
	static  MObject     output; // 出力用のアトリビュート
	static  MTypeId     id; // プラグインのID

private:
	double doCalculation(const double& a, const double& b);
	double cachedValue;
	bool isCacheValid;
};

// 静的な変数宣言
MTypeId     HelloNode::id(0x80001);
MObject     HelloNode::input1;
MObject     HelloNode::input2;
MObject     HelloNode::output;

// クラスの定義
HelloNode::HelloNode() : isCacheValid(false), cachedValue(0.0)
{
}

HelloNode::~HelloNode()
{
}

// 重要な部分その１：アトリビュートを作る！
MStatus HelloNode::initialize()
{
	MFnNumericAttribute nAttr;

	// １．アトリビュートの生成
	input1 = nAttr.create("input1", "in1", MFnNumericData::kDouble, 0.0);
	input2 = nAttr.create("input2", "in2", MFnNumericData::kDouble, 0.0);
	output = nAttr.create("output", "out", MFnNumericData::kDouble, 0.0);

	// ２．アトリビュートの設定、書き込み可能等の設定ができる
	nAttr.setWritable(false);
	nAttr.setStorable(false);

	// ３．アトリビュートを追加します。作っても追加しなければ表示されません。
	addAttribute(input1);
	addAttribute(input2);
	addAttribute(output);

	// ４．アトリビュートの影響を設定。input1が変更されればoutputに影響を与える。
	attributeAffects(input1, output);
	attributeAffects(input2, output);

	return MS::kSuccess;
}


// 重要な部分その２：ノードの計算部分！
MStatus HelloNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus stat;
	if (plug != output) {
		return MS::kUnknownParameter;
	}

	double input1_data = data.inputValue(input1).asDouble();
	double input2_data = data.inputValue(input2).asDouble();
	double output_value;
	
	if (data.context().isNormal())
	{
		if(!isCacheValid)
		{
			cachedValue = doCalculation(input1_data, input2_data);
			isCacheValid = true;
		}
		output_value = cachedValue;
	}
	else
	{
		output_value = doCalculation(input1_data, input2_data);
	}

	data.outputValue(output).set(output_value);
	data.setClean(plug);

	return MS::kSuccess;
}


// 上記の実装で使われる、加算関数
double HelloNode::doCalculation(const double& a, const double& b)
{
	return a + b;
}

// おまけ：プラグがダーティーになったかDG版
MStatus HelloNode::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
	if (plug == input1 || plug == input2)
	{
		MGlobal::displayInfo("MGlobal::setDependentsDirty");
		isCacheValid = false;
	}
	return MS::kSuccess;
}

// おまけ：プラグがダーティーになったかパラレル版
MStatus HelloNode::preEvaluation(const  MDGContext& context, const MEvaluationNode& evaluationNode)
{
	if (context.isNormal())
	{
		if (evaluationNode.dirtyPlugExists(input1) || evaluationNode.dirtyPlugExists(input2))
		{
			MGlobal::displayInfo("MGlobal::preEvaluation");
			isCacheValid = false;
		}
	}
	return MS::kSuccess;
}
void* HelloNode::creator()
{
	return new HelloNode();
}

// プラグインの登録部分
MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, "3dcgbook.inc", "1.0", "Any");
	status = plugin.registerNode(
		"HelloNode",
		HelloNode::id, HelloNode::creator,
		HelloNode::initialize
	);
	return status;
}

// プラグインの解除部分
MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);
	status = plugin.deregisterNode(HelloNode::id);
	return status;
}