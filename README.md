Database 6
================
简介
------------
Database 6 是一个指令驱动的简易数据库，是对前一版本 Database 5 的重构和优化。

指令
------------
Database 6 通过 **指令** 来实现与用户的交互。绝大多数的指令采用如下语法：
> db *指令名* *键值对1* *键值对2* ... *键值对n* *标签1* *标签2*... *标签n*

*键值对* 可以展开为如下格式：
> *键* *值*

键值对作为一个整体，可以与其他键值对或标签交换顺序，只要键值对内部的顺序不变，指令就依然合法且含义不变。例如：
```
db add-field name "Hello World" constr "value>0" unique
```
指令中的`add-field`是指令名，`name "Hello World"`和`constr "value>0"`是键值对，`unique`是标签。根据上面的规则，这条指令与如下指令相等价。
```
add-field unique constr "value>0" name "Hello World" 
```
简洁起见，下面的指令只列出一种可行的排列顺序。

少数指令采用另一种语法：
> *指令名* *参数1* *参数2* ... *参数n*

例如:
```
db add-record 0 1 "Hi"
```
在必要时应为参数添加引号，这点遵循用户使用的bash的语法要求。包含词法或语法错误的指令不能执行，且会引发警告，提示错误原因。

表达式
----------
**表达式**是按一定格式书写以便程序求值的字符串。Database 6 表达式的格式类似于 C 或 C++ 中的表达式，靠**操作符**和**函数**来表达运算，靠**预定义常量**来实现值的替换。

字面量
----------
字面量分为数字型和字符串型。数字型字面量不需要引号、不支持科学计数法，例如：`1`、`3.2`。

字符串型字面量需要引号。例如`"Hello world"`、`"你好"`。由于表达式本身往往被当作指令的参数输入，而引号又是bash保留的字符。所以为了消除歧义，一般要用转义符`\`来确保引号是参数的一部分。

例如：
```
db list-record filter "value(\"学号\")%2=0"
```
由于外层的引号，表达式`value("学号")%2=0`被写作`value(\"学号\")%2=0`。若不加外层引号，则bash可能会提示语法错误。

操作符
----------

操作符分为单目操作符和双目操作符。单目运算符写在操作数左边，例如`-3`、`!0`。双目操作符写在左右操作数之间，例如`2*5`。每个操作符都具有**优先级**，优先级小的操作优先计算。

下面列出了全部的操作符。

操作符|操作|操作数要求|优先级
:-: | :-: | :-: | :-: 
!|取反|数字|2
*|求积|数字|3
/|求商|数字|3
%|取模|数字|3
+|求和或拼接字符串|任意|4
-|求差|数字|4
\>=|判断是否大于或等于|数字|6
<=|判断是否小于或等于|数字|6
=或==|判断数字或字符串是否相等|任意|7
!=|判断数字或字符串是否不相等|任意|7
&或&&|且|数字|11
\|或\|\||或|数字|12
,|连接参数|任意|15

操作数要求标记为“数字”的操作符会在计算前检查操作数是否能被转化为数字，如果不行，就返回空值`null`。

函数
----------
函数以*函数名*打头，后跟以一对圆括号`()`包裹的参数列表。参数之间用`,`隔开。格式如下：
> *函数名*(*参数1*,*参数2*,...,*参数n*)

以下是一般语境下支持的函数：
函数|功能|参数
:-: | :-: | :-: 
`floor(n)`|去除`n`的所有小数位|`n`是数字
`round(n)`|对`n`四舍五入至整数|`n`是数字
`match(s,r)`|返回字符串`s`是否能被正则表达式`r`匹配一次或以上|`s`和`r`是字符串
`strlen(s)`|返回字符串`s`中字符的个数|`s`是字符串

请注意，在特定语境下可能会提供**语境关联**的函数来完成特定的任务,具体信息参见下文。

预定义常量
----------
预定义常量在一次计算中有确定的值，可以用来充当操作数或函数的参数。

以下是通用的预定义常量：
预定义常量|值
:-: | :-: 
pi|3.1416

请注意，在特定语境下可能会提供**语境关联**的预定义常量来完成特定的任务,具体信息参见下文。


字段
----------
**字段**是数据库的列，每一个字段都具有五个属性：**字段名**、**约束**、**格式**、**唯一性** 和 **提示信息**。

字段名是字段的唯一标识符，不可为空，不可与其他字段重复。
 
约束是字段中的数据必须满足的表达式。不满足此表达式的数据无法被输入数据库中；字段的约束也无法设置为其中已有数据不满足的表达式。约束为空的字段可以接受任何数据。

格式是数据显示时计算的表达式，计算的结果会替代数据原值被显示。格式为空时，数据原值会被显示。

约束和格式表达式都可以通过预定义常量`value`来取得记录的值。

唯一性是一个布尔值（真或假,在指令中用`0`或`1`表示），唯一性为真的字段不接受重复的数据。字段中存在重复数据时也不能将字段唯一性修改为真。

提示信息是对字段的注释，方便录入人员理解字段的含义和要求。

添加字段
------------
用`add-field`指令添加字段：
> db add-field <name *字段名*> [constr *约束*] [format *格式*] [info *提示信息*] [unique]

（尖括号`<>`中的项目必选，方括号`[]`中可选，*斜体*为占位符，后同）

可选的项目如果省略，表示对应的字段属性为空（或假），例如：
```
db add-field name "得分" constr "value>=0&value<=100" format "value+\"分\""
```
这条指令将创建一个名为“得分”的字段。该字段限制数据必须是一个在0-100之间的数。显示时，字段格式会在末尾加上“分”字。没有`unique`标签，说明字段中可以包含重复数据。

列出记录
------------
用`list-field`指令列出全部记录。
> db list-field [detailed]

`detailed`表示列出详细信息，无此标签则仅输出字段名和提示信息。

更新字段
------------
用`update-field`指令更新字段的属性：
> db update-field <name *字段名*> [set-name *新字段名*] [set-constr *新约束* | disable-constr] [set-format *新格式* | disable-format] [set-info *新提示信息* | disable-info] [set-unique *新唯一性* | disable-unique]

（`|`表示二选一）

方括号`[]`中的`set-X Y`键值对表示将字段的X属性修改为Y，而与之相对的`disable-X`标签表示将字段的X属性设置为空（或假）。对于每个不同的属性X，`set-X Y`和`disable-X`只能出现其一。特别地，`set-unique 0`和`disable-unique`是等价的。

例如：
```
db update-field name "ID" set-name "学号" set-constr "match(value,\"[0-9]{8}\")" disable-format set-unique 1
```
这条指令将字段“ID”重命名为“学号”、约束设置为满足正则表达式`[0-9]{8}`（8位数字）的字符串、将格式设置为空（禁用格式）并且将唯一性设置为真。

删除字段
------------

用`remove-field`指令来删除字段：
> db remove-field name *字段名*
字段中的全部数据将一并删除。

记录
------------
**记录**是数据库的行。记录中的每个单元格都对应一列，即一个字段。

添加记录
------------
用`add-record`指令来添加一条记录：
> db add-record *值1* *值2* ... *值n*

每个值对应一个字段。如果n小于字段数，缺少的值会记作空，如果n大于字段数，多出的值会被忽略。

列出记录
------------
用`list-record`指令列出记录信息：
> db list-record [filter *筛选条件* | field *字段名* is *值* | field *字段名* within *范围*] [sort *排序方式* | sort-ascending *字段名* | sort-descending *字段名*] [raw] [detailed]

用`field <字段名>`配合`is <值>`或`within <范围>`来针对某字段的值进行筛选。

`field <字段名>`指定了一个字段。`is <值>`表示只列出此字段的值**等于**给定值的记录。例如：
```
db list-record field "性别" is "女"
```
将列出“性别”字段为“女”的记录。

`within <范围>`表示只列出此字段的值**在给定范围内**的记录。范围的格式如下：
> m*最小值*M*最大值*

`m最小值`和`M最大值`的顺序任意，且可以省略其一。例如：
```
db list-record field "学号" within "m6M10"
```
将列出“学号”字段大于等于6且小于等于10的记录。

除此之外，还可以用`filter <筛选条件>`自定义筛选条件。在筛选条件中，可以使用语境关联的函数`value(s)`来取值。
函数|功能|参数
:-: | :-: | :-: 
`value(s)`|取得字段`s`的值|`s`是字符串。
例如：
```
db list-record filter "value(\"学号\")%2=0"
```
将列出所有“学号”能被2整除的记录。 

`filter <筛选条件>`、`field <字段名> is <值>`以及`field <字段名> within <范围>`三者不能混用。

`sort-ascending <字段名>`可以让列出的记录按升序排序，排序标准是指定的字段名。

`sort-decending <字段名>`可以让列出的记录按降序排序，排序标准是指定的字段名。

例如：
```
db list-record sort-ascending "学号"
```
可以列出所有记录，按“学号”升序排序。

除此之外，还可以用`sort <排序方式>`自定义排序方式。在排序方式中，可以使用语境关联的函数`valuea(s)`来取得应当排在前面的值，或用`valueb(s)`来取得应当排在后面的值。

函数|功能|参数
:-: | :-: | :-: 
`valuea(s)`|取得应当排在前面的记录中字段`s`的值|`s`是字符串。
`valueb(s)`|取得应当排在后面的记录中字段`s`的值|`s`是字符串。

例如：
```
db list-record sort "valuea(\"班级\")\<valueb(\"班级\")|valuea(\"班级\")\=valueb(\"班级\")&valuea(\"学号\")<valueb(\"学号\")"
```

表示首先按“班级”字段升序排序，若“班级”相等，就按“学号”字段升序排序
(假设班级、学号都是数字)。

`sort <排序方式>`、`sort-ascending <字段名>`以及`sort-decending <字段名>`三者不能混用。

`raw`标签表示不套用格式，无此标签则将套用格式。

`detailed`标签表示按长格式输出完整数据，无此标签则以表格格式输出。

修改记录
------------
用`update-record`指令来将筛选出的记录的某个字段赋予新值。
> db update-record [filter *筛选条件* | field *字段名* is *值* | field *字段名* within *范围* set *字段名* to *新值*

关于筛选，参见“列出记录”。

`set <字段名>`指定需要赋值的字段，`to <新值>`指定要赋的值。

删除记录
------------
用`remove-record`指令来删除筛选出的记录：
> db remove-record [filter *筛选条件* | field *字段名* is *值* | field *字段名* within *范围*]

关于筛选，参见“列出记录”。


文件相关
------------
你可以从硬盘中读取数据库文件，也可以将当前数据库写入硬盘。

载入文件
-----------
用`load`指令载入文件:
> db load file *路径*

文件路径可以是相对路径或绝对路径。

读取文件
-----------
用`save`指令读取文件：
> db save file *路径*

文件路径可以是相对路径或绝对路径。

清理缓存
-----------
用`clean`指令清除缓存（/tmp/.db_staged）。这会使得未保存的工作全部清空。
> db clean