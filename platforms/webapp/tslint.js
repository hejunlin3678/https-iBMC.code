return {
   /**
    * 说明： 此文件只为开发对照tslint规则，不在任何地方引用
    * 
    * 以下几项根据产品特点通过讨论决定修改
    * "no-bitwise": false, // 禁止按位运算符。？？？？>> || &  h5部分有大量的位运算符
    * "indent": 配置为4个空格缩进
    * variable-name  变量名称检查
    * "ordered-imports": false 针对import的顺序不做要求，因为需要把相关的写在一起
    * trailing-comma  新增的对象/参数等最后一个逗号不加
    */
    "arrow-return-shorthand": true, // convert () => { return x; }  to () => x
    "callable-types": true, // 可以将仅带有调用签名的接口或文字类型编写为函数类型。
    "class-name": true, // 强制类名和接口名称大驼峰
    "comment-format": [ // 单行注释必须有空格
        true,
        "check-space"
    ],
    "curly": true, // 强制执行的括号if/ for/ do/ while语句。 if (x > 0) { doStuff(); }
    "deprecation": {  // 废弃api被使用
      "severity": "warn"
    },
    "eofline": true, // 每个文件以空行结尾
    "forin": true,  // for ... in statement to be filtered with an if statement.
    /**
     * for (let key in someObject) {
     *   if (someObject.hasOwnProperty(key)) {
     *      // code here
     *   }
     * }
     */
    "import-blacklist": [
      true,
      "rxjs/Rx"
    ],
    "import-spacing": true,
    "indent": [ // 空格缩进
      true,
      "spaces"
    ],
    "interface-over-type-literal": true, // 优先于类型文字（type T = { ... }）的接口声明  都使用ineterface就没问题
    "label-position": true, // 此规则仅允许标签位于do/for/while/switch语句上。
    "max-line-length": [ // 每行最大长度
      true,
      140
    ],
    "member-access": false, // 类的成员必须显式声明
    "member-ordering": [ // 类成员的先后顺序
      true,
      {
        "order": [
          "static-field", // 静态属性优先
          "instance-field", // 实例属性
          "static-method", // 静态方法
          "instance-method" // 实例方法
        ]
      }
    ],
    "no-arg": true, // 不允许使用arguments.callee
    "no-bitwise": false, // 禁止按位运算符。？？？？>> || &
    "no-console": [
      true,
      "debug",
      "info",
      "time",
      "timeEnd",
      "trace"
    ],
    "no-construct": true, // 禁止new String()  new Number()  new Boolean()   但不禁止Number(foo)
    "no-debugger": true,
    "no-duplicate-super": true,
    "no-empty": false,   // 禁止空块。内部带有注释的块不被视为空白。
    "no-empty-interface": true,
    "no-eval": true,
    "no-inferrable-types": [ // 不允许为初始化为数字，字符串或布尔值的变量或参数进行显式类型声明。
      false,
      "ignore-params" // 允许为函数参数指定不可推断的类型注释
    ],
    "no-misused-new": true, // 警告为接口或new类定义构造函数的明显尝试 ???  interface{  aa:new xxx()}
    "no-non-null-assertion": true, // 禁止使用!后缀运算符进行非null断言。
    /**
     * function foo(instance: MyClass | undefined) {
     *      instance!.doWork();
     * }
     * 建议
     * function foo(instance: MyClass | undefined) {
     *      if (instance !== undefined) {
     *          instance.doWork();
     *      }  
     * }
     */
    "no-shadowed-variable": true, // 禁止阴影变量声明。
    "no-string-literal": false, // 禁止不必要的字符串文字属性访问。允许obj["prop-erty"]（不能是常规资源访问权限）。不允许obj["property"]（应为obj.property）。
    "no-string-throw": true, // 抛出普通字符串或字符串串联的标志。 建议throw new Error("How can I add new product when no value provided?"); 而不是throw ("How can I add new product when no value provided?");
    "no-switch-case-fall-through": true, // 禁止掉入案例陈述。？？
    "no-trailing-whitespace": true, // 在行尾不允许尾随空格。
    "no-unnecessary-initializer": true, // 禁止将'var'/'let'语句或解构初始化程序初始化为'undefined'。
    "no-unused-expression": true, // 禁止使用未使用的表达式语句。定义了必须用负责不要定义
    "no-use-before-declare": true,
    "no-var-keyword": true,
    "object-literal-sort-keys": false, // 检查对象文字中键的顺序。
    "one-line": [ // 要求指定的标记与它们前面的表达式在同一行。
      true,
      "check-open-brace", // 检查左括号是否与其前一个表达式位于同一行。
      "check-catch",
      "check-else",
      "check-whitespace" //  检查前面的空格是否有指定的标记。
    ],
    "prefer-const": true,
    "quotemark": [
      true,
      "single"
    ],
    "radix": true, // parseInt(XX, 10)
    "semicolon": [ // 在每个语句的末尾强制使用一致的分号。
      true,
      "always"
    ],
    "triple-equals": [ // === 和 !== 代替 == 和 != 
      true,
      "allow-null-check"
    ],
    "typedef-whitespace": [ // 要求或不允许为类型定义使用空格。
      true,
      {
        "call-signature": "nospace", // 检查函数的返回类型。
        "index-signature": "nospace", // 检查索引器的索引类型说明符。
        "parameter": "nospace", // 检查功能参数。
        "property-declaration": "nospace", //  检查对象属性声明。
        "variable-declaration": "nospace" // 检查变量声明。
      }
    ],
    "unified-signatures": true, // 警告可以通过使用并集或可选/其余参数合并为一个的任何两个重载。
    "variable-name": {
      "options": [
        "ban-keywords", // 不允许将某些TypeScript关键字用作变量或参数名称。
        "check-format", // 可以强制执行某种命名格式。默认情况下，该规则仅允许LowerCamelCased或UPPER_CASED变量名。
        "allow-leading-underscore", // 允许在开头加下划线
        "require-const-for-all-caps" // 强制所有名称为UPPER_CASED的变量都应为const。
      ]
    }, // 检查变量名称是否存在各种错误。???
    "whitespace": [ // 强制使用空白样式约定。
      true,
      "check-branch", // （if/ else/ for/ while）后跟空白。
      "check-decl", // 变量声明在equals标记周围是否有空格。
      "check-operator", // 操作符周围的空格。
      "check-separator", // 分隔符（,/ ;）之后检查空格。
      "check-type" // 变量类型指定之前检查空格。
    ],
    "no-output-on-prefix": true,
    "no-inputs-metadata-property": true,
    "no-outputs-metadata-property": true,
    "no-host-metadata-property": false,
    "no-input-rename": true,
    "no-output-rename": true,
    "use-lifecycle-interface": false,
    "use-pipe-transform-interface": true,
    "component-class-suffix": true,
    "directive-class-suffix": true,
    "trailing-comma": false, // 对象/参数等最后一个逗号不加
		"ordered-imports": false // import的排序不作要求
}