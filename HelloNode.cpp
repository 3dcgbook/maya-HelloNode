#include <maya/MPxNode.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEvaluationNode.h>
#include <maya/MEvaluationManager.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

class HelloNode : public MPxNode
{
public:
	HelloNode();
	~HelloNode() override;
	MStatus     compute(const MPlug& plug, MDataBlock& data) override;
	MStatus     setDependentsDirty(const MPlug& plug, MPlugArray& plugArray) override;
	MStatus     preEvaluation(const  MDGContext& context, const MEvaluationNode& evaluationNode) override;
	static  void*       creator();
	static  MStatus     initialize();
public:
	static  MObject     input1; // ���͂P�̃A�g���r���[�g
	static  MObject     input2; // ���͂Q�̃A�g���r���[�g
	static  MObject     output; // �o�͗p�̃A�g���r���[�g
	static  MTypeId     id; // �v���O�C����ID
private:
	double doCalculation(double& a, double& b);
};

// �ÓI�ȕϐ��錾
MTypeId     HelloNode::id(0x80001);
MObject     HelloNode::input1;
MObject     HelloNode::input2;
MObject     HelloNode::output;

// �N���X�̒�`
HelloNode::HelloNode()
{
}

HelloNode::~HelloNode()
{
}

// �d�v�ȕ������̂P�F�A�g���r���[�g�����I
MStatus HelloNode::initialize()
{
	MFnNumericAttribute nAttr;

	input1 = nAttr.create("input1", "in1", MFnNumericData::kDouble, 0.0);
	input2 = nAttr.create("input2", "in2", MFnNumericData::kDouble, 0.0);
	output = nAttr.create("output", "out", MFnNumericData::kDouble, 0.0);

	nAttr.setWritable(false);
	nAttr.setStorable(false);

	addAttribute(input1);
	addAttribute(input2);
	addAttribute(output);

	attributeAffects(input1, output);
	attributeAffects(input2, output);

	return MS::kSuccess;
}


// �d�v�ȕ������̂Q�F�m�[�h�̌v�Z�����I
MStatus HelloNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus stat;
	if (plug != output) {
		return MS::kUnknownParameter;
	}
	
	double input1_data = data.inputValue(input1).asDouble();
	double input2_data = data.inputValue(input2).asDouble();
		
		
	double output_value = doCalculation(input1_data, input2_data);
		
	data.outputValue(HelloNode::output).set(output_value);
	data.setClean(plug);

	return MS::kSuccess;
}


// ��L�̎����Ŏg����A���Z�֐�
double HelloNode::doCalculation(double& a, double& b)
{
	return a + b;
}

// ���܂��F�v���O���_�[�e�B�[�ɂȂ�����DG��
MStatus HelloNode::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
	if (plug == input1 || plug == input2)
	{	
		MGlobal::displayInfo("MGlobal::setDependentsDirty");
	}
	
	return MS::kSuccess;
}

// ���܂��F�v���O���_�[�e�B�[�ɂȂ������p��������
MStatus HelloNode::preEvaluation(const  MDGContext& context, const MEvaluationNode& evaluationNode)
{
	if (context.isNormal())
	{
		if (evaluationNode.dirtyPlugExists(input1) || evaluationNode.dirtyPlugExists(input2))
		{
			MGlobal::displayInfo("MGlobal::preEvaluation");
		}
	}
	return MS::kSuccess;
}
void* HelloNode::creator()
{
	return new HelloNode();
}

// �v���O�C���̓o�^����
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

// �v���O�C���̉�������
MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);
	status = plugin.deregisterNode(HelloNode::id);
	return status;
}