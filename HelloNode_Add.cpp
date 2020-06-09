#include <maya/MPxNode.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnPlugin.h>

class HelloNode : public MPxNode
{
public: // 関数関連
	HelloNode();
	virtual ~HelloNode();
	
	virtual MStatus     compute(const MPlug& plug, MDataBlock& data);
	static  void*       creator();
	static  MStatus     initialize();

public: // プラグインIDのアトリビュートを入れるところ
	static  MTypeId     id; // プラグインのID
	static  MObject     input1; // 入力１のアトリビュート
	static  MObject     input2; // 入力２のアトリビュート
	static  MObject     output; // 出力用のアトリビュート
	
};


// 静的な変数宣言
MTypeId     HelloNode::id(0x80002);
MObject     HelloNode::input1;
MObject     HelloNode::input2;
MObject     HelloNode::output;


// クラスの定義
HelloNode::HelloNode()
{
}


HelloNode::~HelloNode()
{
}


void* HelloNode::creator()
{
	return new HelloNode();
}


// 重要な部分その１：アトリビュートを作る！
MStatus HelloNode::initialize()
{
	MFnNumericAttribute nAttr;

	// １．アトリビュートの生成
	input1 = nAttr.create("input1", "in1", MFnNumericData::kDouble, 0.0);
	input2 = nAttr.create("input2", "in2", MFnNumericData::kDouble, 0.0);
	output = nAttr.create("output", "out", MFnNumericData::kDouble, 0.0);

	// ２．アトリビュートの設定、書き込み可能等の設定ができる。outputのアトリビュート設定を行っている。
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
	double output_value = input1_data + input2_data;
	data.outputValue(output).set(output_value);
	data.setClean(plug);
	return MS::kSuccess;
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