/**
 * Created by Administrator on 2016/5/6.
 */
; ! window['qcVideo'] && (function(global) {
    var ns = {
            modules: {},
            instances: {}
        },
        waiter = {};
    function getMappingArgs(fn) {
        var args = fn.toString().split('{')[0].replace(/\s|function|\(|\)/g, '').split(','),
            i = 0;
        if (!args[0]) {
            args = [];
        }
        while (args[i]) {
            args[i] = require(args[i]);
            i += 1;
        }
        return args;
    }
    function newInst(key, ifExist) {
        if ((ifExist ? ns.instances[key] : !ns.instances[key]) && ns.modules[key]) {
            ns.instances[key] = ns.modules[key].apply(window, getMappingArgs(ns.modules[key]));
        }
    }
    function require(key) {
        newInst(key, false);
        return ns.instances[key] || {};
    }
    function loadJs(url) {
        var el = document.createElement('script');
        el.setAttribute('type', 'text/javascript');
        el.setAttribute('src', url);
        el.setAttribute('async', true);
        document.getElementsByTagName("head")[0].appendChild(el);
    }
    function core(key, target) {
        if (!ns.modules[key]) {
            ns.modules[key] = target;
            newInst(key, true);
            if ( !! waiter[key]) {
                var i = 0;
                while (waiter[key][i]) {
                    waiter[key][i](require(key));
                    i += 1;
                }
                delete waiter[key];
            }
        }
    }
    core.use = function(key, cb) {
        cb = cb ||
        function() {};
        if (ns.modules[key]) {
            cb(require(key));
        } else {
            var config = require('config');
            if (config[key]) {
                if (!waiter[key]) {
                    waiter[key] = [];
                    loadJs(config[key]);
                }
                waiter[key].push(cb);
            }
        }
    };
    core.get = function(key) {
        return require(key);
    };
    core.loadJs = loadJs;
    global.qcVideo = core;
})(window);
qcVideo('Base',
    function(util) {
        var unique = 'base_' + ( + new Date()),
            global = window,
            uuid = 1,
            Base = function() {},
            debug = true,
            realConsole = global.console,
            console = realConsole || {},
            wrap = function(fn) {
                return function() {
                    if (debug) {
                        try {
                            fn.apply(realConsole, [this.__get_class_info__()].concat(arguments));
                        } catch(xe) {}
                    }
                }
            };
        Base.prototype.__get_class_info__ = function() {
            var now = new Date();
            return now.getHours() + ':' + now.getMinutes() + ':' + now.getSeconds() + '>' + (this['className'] || 'BASE') + '>';
        };
        Base.setDebug = function(open) {
            debug = !!open;
        };
        Base.filter_error = function(fn, name) {
            if (util.type(fn) != 'function') {
                return fn;
            }
            return function() {
                try {
                    return fn.apply(this, arguments);
                } catch(xe) {
                    var rep = qcVideo.get('BJ_REPORT');
                    if (rep && rep.push) {
                        if (xe.stack) {
                            xe.stack = (this.className || '') + '-' + (name || 'constructor') + ' ' + xe.stack;
                        }
                        rep.push(xe);
                    }
                    throw new Error(xe.message || '');
                }
            };
        };
        Base.prototype.loop = Base.loop = function() {};
        Base.extend = function(protoProps, staticProps) {
            protoProps = protoProps || {};
            var constructor = protoProps.hasOwnProperty('constructor') ? protoProps.constructor: function() {
                return sup.apply(this, arguments);
            };
            constructor = Base.filter_error(constructor);
            var sup = this,
                kk;
            var Fn = function() {
                this.constructor = constructor;
            };
            if (protoProps) {
                for (kk in protoProps) {
                    protoProps[kk] = Base.filter_error(protoProps[kk], kk);
                }
            }
            Fn.prototype = sup.prototype;
            constructor.prototype = new Fn();
            util.merge(constructor.prototype, protoProps);
            util.merge(constructor, sup, true);
            util.merge(constructor, staticProps);
            util.merge(constructor, {
                __super__: sup.prototype
            });
            return constructor;
        };
        Base.prototype.log = wrap(console.log || Base.loop);
        Base.prototype.debug = wrap(console.debug || Base.loop);
        Base.prototype.error = wrap(console.error || Base.loop);
        Base.prototype.info = wrap(console.info || Base.loop);
        var eventCache = {};
        var getUniqueId = function() {
            return this.__id || (this.__id = unique + (++uuid));
        };
        var initEvent = function(ctx, event) {
            var id = getUniqueId.call(ctx);
            if (!eventCache.hasOwnProperty(id)) {
                eventCache[id] = {};
            }
            if (event) {
                if (!eventCache[id][event]) {
                    eventCache[id][event] = [];
                }
            }
        };
        Base.prototype.on = function(ctx, event, fn) {
            initEvent(ctx, event);
            eventCache[getUniqueId.call(ctx)][event].push(fn);
        };
        Base.prototype.batchOn = function(ctx, ary) {
            for (var i = 0,
                     j = ary.length; i < j; i++) {
                this.on(ctx, ary[i]['event'], ary[i]['fn'])
            }
        };
        Base.prototype.fire = function(event, opt) {
            var cache = eventCache[getUniqueId.call(this)];
            if (cache && cache[event]) {
                util.each(cache[event],
                    function(fn) {
                        fn.call(global, opt);
                    });
            }
        };
        Base.prototype.off = function(ctx, event, fn) {
            initEvent(ctx);
            var find = -1,
                list = eventCache[getUniqueId.call(ctx)][event];
            util.each(list,
                function(handler, index) {
                    if (handler === fn) {
                        find = index;
                        return false;
                    }
                });
            if (find !== -1) {
                list.splice(find, 1);
            }
        };
        Base.instance = function(opt, staticOpt) {
            return new(Base.extend(opt, staticOpt))();
        };
        return Base;
    });;
qcVideo('tlsPwd',
    function() {
        function Now() {
            return + new Date();
        }
        function addTlsScript() {
            var a = document.createElement('script');
            a.src = 'https://tls.qcloud.com/libs/encrypt.min.js';
            document.body.insertBefore(a, document.body.childNodes[0]);
        }
        function getSigPwd() {
            try {
                return Encrypt.getRSAH1();
            } catch(e) {}
            return '';
        }
        var getSigPwdStartTime;
        function fetchSigPwd(cb, start) {
            var now = Now();
            if (start) {
                getSigPwdStartTime = now;
                addTlsScript();
            } else {
                if (now - getSigPwdStartTime > 5000) {
                    cb(null, 'timeout');
                    return;
                }
            }
            var pwd = getSigPwd();
            if (pwd && pwd.length > 0) {
                cb(pwd);
            } else {
                setTimeout(function() {
                        fetchSigPwd(cb);
                    },
                    1000);
            }
        }
        return function(cb) {
            fetchSigPwd(function(pwd) {
                    cb(pwd);
                },
                true);
        };
    });;
qcVideo('touristTlsLogin',
    function(tlsPwd) {
        var global = window;
        function askJsonp(src) {
            var a = document.createElement('script');
            a.src = src;
            document.body.insertBefore(a, document.body.childNodes[0]);
        }
        function tlsGetUserSig_JsonPCallback(info) {
            info = info || {};
            var ErrorCode = info['ErrorCode'];
            clear_jsonP();
            if (ErrorCode == 0) {
                _info['userSig'] = info['UserSig'];
                _info.done(_info);
            } else {
                _info.done(null, ErrorCode);
            }
        }
        function clear_jsonP() {
            global.tlsAnoLogin = null;
            global.tlsGetUserSig = null;
        }
        function tlsAnoLogin_JsonPCallback(info) {
            info = info || {};
            var ErrorCode = info['ErrorCode'];
            if (ErrorCode == 0) {
                _info['identifier'] = info['Identifier'];
                _info['TmpSig'] = info['TmpSig'];
                global.tlsGetUserSig = tlsGetUserSig_JsonPCallback;
                askJsonp('https://tls.qcloud.com/getusersig?tmpsig=' + _info.TmpSig + '&identifier=' + encodeURIComponent(_info.identifier) + '&accounttype=' + _info['accountType'] + '&sdkappid=' + _info['sdkAppID']);
            } else {
                clear_jsonP();
                _info.done(null, ErrorCode);
            }
        }
        var _info = {};
        return function(sdkappid, accounttype, cb) {
            _info = {
                sdkAppID: sdkappid,
                appIDAt3rd: sdkappid,
                accountType: accounttype,
                identifier: '',
                userSig: '',
                done: cb
            };
            clear_jsonP();
            tlsPwd(function(pwd, error) {
                if (error) {
                    _info.done(null, error);
                }
                askJsonp('https://tls.qcloud.com/anologin?sdkappid=' + _info['sdkAppID'] + '&accounttype=' + _info['accountType'] + '&url=&passwd=' + pwd);
                global.tlsAnoLogin = tlsAnoLogin_JsonPCallback;
            });
        };
    });;
qcVideo('api',
    function() {
        var now = function() {
                return + new Date();
            },
            uuid = 0,
            global = window,
            unique = 'qcvideo_' + now(),
            overTime = 10000;
        var request = function(address, cbName, cb) {
            return function() {
                global[cbName] = function(data) {
                    cb(data);
                    delete global[cbName];
                };
                setTimeout(function() {
                        if (typeof global[cbName] !== "undefined") {
                            delete global[cbName];
                            cb({
                                'retcode': 10000,
                                'errmsg': '请求超时'
                            });
                        }
                    },
                    overTime);
                qcVideo.loadJs(address + (address.indexOf('?') > 0 ? '&': '?') + 'callback=' + cbName);
            }
        };
        var hiSender = function() {
            var img = new Image();
            return function(src) {
                img.onload = img.onerror = img.onabort = function() {
                    img.onload = img.onerror = img.onabort = null;
                    img = null;
                };
                img.src = src;
            };
        };
        var apdTime = function(url) {
            return url + (url.indexOf('?') > 0 ? '&': '?') + '_=' + now();
        };
        return {
            request: function(address, cb) {
                var cbName = unique + '_callback' + (++uuid);
                request(apdTime(address), cbName, cb)();
            },
            report: function(address) {
                hiSender()(apdTime(address));
            }
        };
    });
qcVideo('BJ_REPORT',
    function() {
        return (function(global) {
            if (global.BJ_REPORT) return global.BJ_REPORT;
            var _error = [];
            var _config = {
                id: 0,
                uin: 0,
                url: "",
                combo: 1,
                ext: {},
                level: 4,
                ignore: [],
                random: 1,
                delay: 1000,
                submit: null
            };
            var _isOBJByType = function(o, type) {
                return Object.prototype.toString.call(o) === "[object " + (type || "Object") + "]";
            };
            var _isOBJ = function(obj) {
                var type = typeof obj;
                return type === "object" && !!obj;
            };
            var _isEmpty = function(obj) {
                if (obj === null) return true;
                if (_isOBJByType(obj, 'Number')) {
                    return false;
                }
                return ! obj;
            };
            var orgError = global.onerror;
            var _processError = function(errObj) {
                try {
                    if (errObj.stack) {
                        var url = errObj.stack.match("https?://[^\n]+");
                        url = url ? url[0] : "";
                        var rowCols = url.match(":(\\d+):(\\d+)");
                        if (!rowCols) {
                            rowCols = [0, 0, 0];
                        }
                        var stack = _processStackMsg(errObj);
                        return {
                            msg: stack,
                            rowNum: rowCols[1],
                            colNum: rowCols[2],
                            target: url.replace(rowCols[0], "")
                        };
                    } else {
                        return errObj;
                    }
                } catch(err) {
                    return errObj;
                }
            };
            var _processStackMsg = function(error) {
                var stack = error.stack.replace(/\n/gi, "").split(/\bat\b/).slice(0, 5).join("@").replace(/\?[^:]+/gi, "");
                var msg = error.toString();
                if (stack.indexOf(msg) < 0) {
                    stack = msg + "@" + stack;
                }
                return stack;
            };
            var _error_tostring = function(error, index) {
                var param = [];
                var params = [];
                var stringify = [];
                if (_isOBJ(error)) {
                    error.level = error.level || _config.level;
                    for (var key in error) {
                        var value = error[key];
                        if (!_isEmpty(value)) {
                            if (_isOBJ(value)) {
                                try {
                                    value = JSON.stringify(value);
                                } catch(err) {
                                    value = "[BJ_REPORT detect value stringify error] " + err.toString();
                                }
                            }
                            stringify.push(key + ":" + value);
                            param.push(key + "=" + encodeURIComponent(value));
                            params.push(key + "[" + index + "]=" + encodeURIComponent(value));
                        }
                    }
                }
                return [params.join("&"), stringify.join(","), param.join("&")];
            };
            var _imgs = [];
            var _submit = function(url) {
                if (_config.submit) {
                    _config.submit(url);
                } else {
                    var _img = new Image();
                    _imgs.push(_img);
                    _img.src = url;
                }
            };
            var error_list = [];
            var comboTimeout = 0;
            var _send = function(isReoprtNow) {
                if (!_config.report) return;
                while (_error.length) {
                    var isIgnore = false;
                    var error = _error.shift();
                    var error_str = _error_tostring(error, error_list.length);
                    if (_isOBJByType(_config.ignore, "Array")) {
                        for (var i = 0,
                                 l = _config.ignore.length; i < l; i++) {
                            var rule = _config.ignore[i];
                            if ((_isOBJByType(rule, "RegExp") && rule.test(error_str[1])) || (_isOBJByType(rule, "Function") && rule(error, error_str[1]))) {
                                isIgnore = true;
                                break;
                            }
                        }
                    }
                    if (!isIgnore) {
                        if (_config.combo) {
                            error_list.push(error_str[0]);
                        } else {
                            _submit(_config.report + error_str[2] + "&_t=" + ( + new Date));
                        }
                        _config.onReport && (_config.onReport(_config.id, error));
                    }
                }
                var count = error_list.length;
                if (count) {
                    var comboReport = function() {
                        clearTimeout(comboTimeout);
                        _submit(_config.report + error_list.join("&") + "&count=" + count + "&_t=" + ( + new Date));
                        comboTimeout = 0;
                        error_list = [];
                    };
                    if (isReoprtNow) {
                        comboReport();
                    } else if (!comboTimeout) {
                        comboTimeout = setTimeout(comboReport, _config.delay);
                    }
                }
            };
            var report = {
                push: function(msg) {
                    if (Math.random() >= _config.random) {
                        return report;
                    }
                    _error.push(_isOBJ(msg) ? _processError(msg) : {
                        msg: msg
                    });
                    _send();
                    return report;
                },
                report: function(msg) {
                    msg && report.push(msg);
                    _send(true);
                    return report;
                },
                info: function(msg) {
                    if (!msg) {
                        return report;
                    }
                    if (_isOBJ(msg)) {
                        msg.level = 2;
                    } else {
                        msg = {
                            msg: msg,
                            level: 2
                        };
                    }
                    report.push(msg);
                    return report;
                },
                debug: function(msg) {
                    if (!msg) {
                        return report;
                    }
                    if (_isOBJ(msg)) {
                        msg.level = 1;
                    } else {
                        msg = {
                            msg: msg,
                            level: 1
                        };
                    }
                    report.push(msg);
                    return report;
                },
                init: function(config) {
                    if (_isOBJ(config)) {
                        for (var key in config) {
                            _config[key] = config[key];
                        }
                    }
                    var id = parseInt(_config.id, 10);
                    if (id) {
                        _config.report = (_config.url || "//badjs2.qq.com/badjs") + "?id=" + id + "&uin=" + parseInt(_config.uin || (document.cookie.match(/\buin=\D+(\d+)/) || [])[1], 10) + "&from=" + encodeURIComponent(location.href) + "&ext=" + JSON.stringify(_config.ext) + "&";
                    }
                    return report;
                },
                __onerror__: global.onerror
            };
            typeof console !== "undefined" && console.error && setTimeout(function() {
                    var err = ((location.hash || '').match(/([#&])BJ_ERROR=([^&$]+)/) || [])[2];
                    err && console.error("BJ_ERROR", decodeURIComponent(err).replace(/(:\d+:\d+)\s*/g, '$1\n'));
                },
                0);
            return report;
        } (window));
    });
qcVideo('css',
    function() {
        var css = {};
        if (document.defaultView && document.defaultView.getComputedStyle) {
            css.getComputedStyle = function(a, b) {
                var c, d, e;
                b = b.replace(/([A-Z]|^ms)/g, "-$1").toLowerCase();
                if ((d = a.ownerDocument.defaultView) && (e = d.getComputedStyle(a, null))) {
                    c = e.getPropertyValue(b)
                }
                return c
            }
        } else if (document.documentElement.currentStyle) {
            css.getComputedStyle = function(a, b) {
                var c, d = a.currentStyle && a.currentStyle[b],
                    e = a.style;
                if (d === null && e && (c = e[b])) {
                    d = c
                }
                return d
            }
        }
        return {
            getWidth: function(e) {
                return (css.getComputedStyle(e, 'width') || "").toLowerCase().replace('px', '') | 0;
            },
            getHeight: function(e) {
                return (css.getComputedStyle(e, 'height') || "").toLowerCase().replace('px', '') | 0;
            },
            textAlign: function(e) {
                e.style['text-align'] = 'center';
            },
            getVisibleHeight: function() {
                var doc = document;
                var docE = doc.documentElement;
                var body = doc.body;
                return (docE && docE.clientHeight) || (body && body.offsetHeight) || window.innerHeight || 0;
            },
            getVisibleWidth: function() {
                var doc = document;
                var docE = doc.documentElement;
                var body = doc.body;
                return (docE && docE.clientWidth) || (body && body.offsetWidth) || window.innerWidth || 0;
            }
        };
    });;
qcVideo('interval',
    function() {
        var git, stack = {},
            length = 0,
            gTime = 16,
            uuid = 0;
        function each(cb) {
            for (var key in stack) {
                if (false === cb.call(stack[key])) {
                    return;
                }
            }
        }
        function tick() {
            var now = +new Date();
            each(function() {
                var me = this; ! me.__time && (me.__time = now);
                if (me.__time + me._ftp <= now && me.status === 1) {
                    me.__time = now;
                    me._cb.call();
                }
            });
        }
        function stop() {
            var start = 0;
            each(function() {
                this.status === 1 && (start += 1);
            });
            if (start === 0 || length === 0) {
                clearInterval(git);
                git = null;
            }
        }
        function _start() {
            this.status = 1; ! git && (git = setInterval(tick, gTime));
        }
        function _pause() {
            this.status = 0;
            this.__time = +new Date();
            stop();
        }
        function _clear() {
            delete stack[this._id];
            length -= 1;
            stop();
        }
        return function(callback, time) {
            length += 1;
            uuid += 1;
            return stack[uuid] = {
                _id: uuid,
                _cb: callback,
                _ftp: time || gTime,
                start: _start,
                pause: _pause,
                clear: _clear
            };
        };
    }) if (typeof JSON !== 'object') {
    JSON = {};
} (function() {
    'use strict';
    function f(n) {
        return n < 10 ? '0' + n: n;
    }
    if (typeof Date.prototype.toJSON !== 'function') {
        Date.prototype.toJSON = function() {
            return isFinite(this.valueOf()) ? this.getUTCFullYear() + '-' + f(this.getUTCMonth() + 1) + '-' + f(this.getUTCDate()) + 'T' + f(this.getUTCHours()) + ':' + f(this.getUTCMinutes()) + ':' + f(this.getUTCSeconds()) + 'Z': null;
        };
        String.prototype.toJSON = Number.prototype.toJSON = Boolean.prototype.toJSON = function() {
            return this.valueOf();
        };
    }
    var cx, escapable, gap, indent, meta, rep;
    function quote(string) {
        escapable.lastIndex = 0;
        return escapable.test(string) ? '"' + string.replace(escapable,
            function(a) {
                var c = meta[a];
                return typeof c === 'string' ? c: '\\u' + ('0000' + a.charCodeAt(0).toString(16)).slice( - 4);
            }) + '"': '"' + string + '"';
    }
    function str(key, holder) {
        var i, k, v, length, mind = gap,
            partial, value = holder[key];
        if (value && typeof value === 'object' && typeof value.toJSON === 'function') {
            value = value.toJSON(key);
        }
        if (typeof rep === 'function') {
            value = rep.call(holder, key, value);
        }
        switch (typeof value) {
            case 'string':
                return quote(value);
            case 'number':
                return isFinite(value) ? String(value) : 'null';
            case 'boolean':
            case 'null':
                return String(value);
            case 'object':
                if (!value) {
                    return 'null';
                }
                gap += indent;
                partial = [];
                if (Object.prototype.toString.apply(value) === '[object Array]') {
                    length = value.length;
                    for (i = 0; i < length; i += 1) {
                        partial[i] = str(i, value) || 'null';
                    }
                    v = partial.length === 0 ? '[]': gap ? '[\n' + gap + partial.join(',\n' + gap) + '\n' + mind + ']': '[' + partial.join(',') + ']';
                    gap = mind;
                    return v;
                }
                if (rep && typeof rep === 'object') {
                    length = rep.length;
                    for (i = 0; i < length; i += 1) {
                        if (typeof rep[i] === 'string') {
                            k = rep[i];
                            v = str(k, value);
                            if (v) {
                                partial.push(quote(k) + (gap ? ': ': ':') + v);
                            }
                        }
                    }
                } else {
                    for (k in value) {
                        if (Object.prototype.hasOwnProperty.call(value, k)) {
                            v = str(k, value);
                            if (v) {
                                partial.push(quote(k) + (gap ? ': ': ':') + v);
                            }
                        }
                    }
                }
                v = partial.length === 0 ? '{}': gap ? '{\n' + gap + partial.join(',\n' + gap) + '\n' + mind + '}': '{' + partial.join(',') + '}';
                gap = mind;
                return v;
        }
    }
    if (typeof JSON.stringify !== 'function') {
        escapable = /[\\\"\x00-\x1f\x7f-\x9f\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g;
        meta = {
            '\b': '\\b',
            '\t': '\\t',
            '\n': '\\n',
            '\f': '\\f',
            '\r': '\\r',
            '"': '\\"',
            '\\': '\\\\'
        };
        JSON.stringify = function(value, replacer, space) {
            var i;
            gap = '';
            indent = '';
            if (typeof space === 'number') {
                for (i = 0; i < space; i += 1) {
                    indent += ' ';
                }
            } else if (typeof space === 'string') {
                indent = space;
            }
            rep = replacer;
            if (replacer && typeof replacer !== 'function' && (typeof replacer !== 'object' || typeof replacer.length !== 'number')) {
                throw new Error('JSON.stringify');
            }
            return str('', {
                '': value
            });
        };
    }
    if (typeof JSON.parse !== 'function') {
        cx = /[\u0000\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g;
        JSON.parse = function(text, reviver) {
            var j;
            function walk(holder, key) {
                var k, v, value = holder[key];
                if (value && typeof value === 'object') {
                    for (k in value) {
                        if (Object.prototype.hasOwnProperty.call(value, k)) {
                            v = walk(value, k);
                            if (v !== undefined) {
                                value[k] = v;
                            } else {
                                delete value[k];
                            }
                        }
                    }
                }
                return reviver.call(holder, key, value);
            }
            text = String(text);
            cx.lastIndex = 0;
            if (cx.test(text)) {
                text = text.replace(cx,
                    function(a) {
                        return '\\u' + ('0000' + a.charCodeAt(0).toString(16)).slice( - 4);
                    });
            }
            if (/^[\],:{}\s]*$/.test(text.replace(/\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g, '@').replace(/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g, ']').replace(/(?:^|:|,)(?:\s*\[)+/g, ''))) {
                j = eval('(' + text + ')');
                return typeof reviver === 'function' ? walk({
                        '': j
                    },
                    '') : j;
            }
            throw new SyntaxError('JSON.parse');
        };
    }
} ());
qcVideo('JSON',
    function() {
        return JSON;
    });;
qcVideo('LinkIm',
    function(Base, touristTlsLogin) {
        return Base.extend({
            className: 'LinkIm',
            checkLoginStatus: function(uniqueImVal) {
                var s = this.link.checkLoginBarrage(uniqueImVal);
                return s != '0' ? s: 'uninit';
            },
            destroy: function() {
                delete this.link;
                delete this.im;
            },
            constructor: function(link, im, uniqueImVal, done, fail) {
                var self = this;
                self.link = link;
                self.im = im;
                var roll = function() {
                    var status = self.checkLoginStatus(uniqueImVal);
                    if (status == 'uninit') {
                        setTimeout(roll, 1000);
                    } else {
                        if (status == 'fail') {
                            touristTlsLogin(self.im['sdkAppID'], self.im['accountType'],
                                function(info, error) {
                                    if (!error) {
                                        info['groupId'] = self.im['groupId'];
                                        info['nickName'] = self.im['nickName'];
                                        info['appId'] = uniqueImVal;
                                        delete info.done;
                                        delete info.TmpSig;
                                        self.link.loginBarrage(info);
                                        done && done();
                                    } else {
                                        fail && fail('tlsLogin:' + error);
                                    }
                                });
                        } else {
                            self.link.loginBarrage({
                                'appId': uniqueImVal,
                                'groupId': self.im['groupId'],
                                'nickName': self.im['nickName']
                            });
                            done && done();
                        }
                    }
                };
                roll();
            }
        });
    });;
qcVideo('lStore',
    function() {
        var win = window,
            doc = win.document,
            localStorageName = 'localStorage',
            globalStorageName = 'globalStorage',
            storage, get, set, remove, clear, key_prefix = 'qc_video_love_',
            ok = false;
        set = get = remove = clear = function() {};
        try {
            if (localStorageName in win && win[localStorageName]) {
                storage = win[localStorageName];
                set = function(key, val) {
                    storage.setItem(key, val)
                };
                get = function(key) {
                    return storage.getItem(key)
                };
                remove = function(key) {
                    storage.removeItem(key)
                };
                clear = function() {
                    storage.clear()
                };
                ok = true;
            }
        } catch(e) {}
        try {
            if (!ok && globalStorageName in win && win[globalStorageName]) {
                storage = win[globalStorageName][win.location.hostname];
                set = function(key, val) {
                    storage[key] = val
                };
                get = function(key) {
                    return storage[key] && storage[key].value
                };
                remove = function(key) {
                    delete storage[key]
                };
                clear = function() {
                    for (var key in storage) {
                        delete storage[key]
                    }
                };
                ok = true;
            }
        } catch(e) {}
        if (!ok && doc.documentElement.addBehavior) {
            function getStorage() {
                if (storage) {
                    return storage
                }
                storage = doc.body.appendChild(doc.createElement('div'));
                storage.style.display = 'none';
                storage.setAttribute('data-store-js', '');
                storage.addBehavior('#default#userData');
                storage.load(localStorageName);
                return storage;
            }
            set = function(key, val) {
                try {
                    var storage = getStorage();
                    storage.setAttribute(key, val);
                    storage.save(localStorageName);
                } catch(e) {}
            };
            get = function(key) {
                try {
                    var storage = getStorage();
                    return storage.getAttribute(key);
                } catch(e) {
                    return '';
                }
            };
            remove = function(key) {
                try {
                    var storage = getStorage();
                    storage.removeAttribute(key);
                    storage.save(localStorageName);
                } catch(e) {}
            };
            clear = function() {
                try {
                    var storage = getStorage();
                    var attributes = storage.XMLDocument.documentElement.attributes;
                    storage.load(localStorageName);
                    for (var i = 0,
                             attr; attr = attributes[i]; i++) {
                        storage.removeAttribute(attr.name);
                    }
                    storage.save(localStorageName);
                } catch(e) {}
            }
        }
        return {
            get: function(key) {
                return get(key_prefix + key);
            },
            set: function(key, val) {
                set(key_prefix + key, val);
            },
            remove: function(key) {
                remove(key_prefix + key);
            },
            clear: clear
        };
    });;
qcVideo('util',
    function() {
        var util = {
            paramsToObject: function(link) {
                var result = {},
                    pairs, pair, query, key, value;
                query = link || '';
                query = query.replace('?', '');
                pairs = query.split('&');
                for (var i = 0,
                         j = pairs.length; i < j; i++) {
                    var keyVal = pairs[i];
                    pair = keyVal.split('=');
                    key = pair[0];
                    value = pair.slice(1).join('=');
                    result[decodeURIComponent(key)] = decodeURIComponent(value);
                }
                return result;
            },
            each: function(opt, cb) {
                var key = 0,
                    i, j;
                if (this.isArray(opt)) {
                    for (i = 0, j = opt.length; i < j; i++) {
                        if (false === cb.call(opt[i], opt[i], i)) {
                            break;
                        }
                    }
                } else if (this.isPlainObject(opt)) {
                    for (key in opt) {
                        if (false === cb.call(opt[key], opt[key], key)) {
                            break;
                        }
                    }
                }
            }
        };
        var toString = Object.prototype.toString,
            hasOwn = Object.prototype.hasOwnProperty,
            class2type = {
                '[object Boolean]': 'boolean',
                '[object Number]': 'number',
                '[object String]': 'string',
                '[object Function]': 'function',
                '[object Array]': 'array',
                '[object Date]': 'date',
                '[object RegExp]': 'regExp',
                '[object Object]': 'object'
            },
            isWindow = function(obj) {
                return obj && typeof obj === "object" && "setInterval" in obj;
            };
        util.type = function(obj) {
            return obj == null ? String(obj) : class2type[toString.call(obj)] || "object";
        };
        util.isArray = Array.isArray ||
        function(obj) {
            return util.type(obj) === "array";
        };
        util.isPlainObject = function(obj) {
            if (!obj || util.type(obj) !== "object" || obj.nodeType || isWindow(obj)) {
                return false;
            }
            if (obj.constructor && !hasOwn.call(obj, "constructor") && !hasOwn.call(obj.constructor.prototype, "isPrototypeOf")) {
                return false;
            }
            var key;
            for (key in obj) {}
            return key === undefined || hasOwn.call(obj, key);
        };
        util.merge = function(tar, sou, deep) {
            var name, src, copy, clone, copyIsArray;
            for (name in sou) {
                src = tar[name];
                copy = sou[name];
                if (tar !== copy) {
                    if (deep && copy && (util.isPlainObject(copy) || (copyIsArray = util.isArray(copy)))) {
                        if (copyIsArray) {
                            copyIsArray = false;
                            clone = src && util.isArray(src) ? src: [];
                        } else {
                            clone = src && util.isPlainObject(src) ? src: {};
                        }
                        tar[name] = util.merge(clone, copy, deep);
                    } else if (copy !== undefined) {
                        tar[name] = copy;
                    }
                }
            }
            return tar;
        };
        util.capitalize = function(str) {
            str = str || '';
            return str.charAt(0).toUpperCase() + str.slice(1);
        };
        util.convertTime = function(s) {
            s = s | 0;
            var h = 3600,
                m = 60;
            var hours = (s / h) | 0;
            var minutes = (s - hours * h) / m | 0;
            var sec = s - hours * h - minutes * m;
            hours = hours > 0 ? (hours + ':') : '';
            minutes = minutes > 0 ? (minutes + ':') : (hours > 0 ? '00:': '');
            sec = sec > 0 ? (sec + '') : (hours.length > 0 || minutes.length > 0 ? '00': '00:00:00');
            hours = hours.length == 2 ? ('0' + hours) : hours;
            minutes = minutes.length == 2 ? ('0' + minutes) : minutes;
            sec = sec.length == 1 ? ('0' + sec) : sec;
            return hours + minutes + sec
        };
        util.fix2 = function(num) {
            return num.toFixed(2) - 0;
        };
        util.fileType = function(src) {
            if (src.indexOf('.mp4') > 0) {
                return 'mp4'
            }
            if (src.indexOf('.m3u8') > 0) {
                return 'hls';
            }
        };
        util.loadImg = function(url, ready) {
            var onReady, width, height, newWidth, newHeight, img = new Image();
            img.src = url;
            if (img.complete) {
                ready.call(img);
                return;
            }
            width = img.width;
            height = img.height;
            img.onerror = function() {
                onReady.end = true;
                img = img.onload = img.onerror = null;
            };
            onReady = function() {
                newWidth = img.width;
                newHeight = img.height;
                if (newWidth !== width || newHeight !== height || newWidth * newHeight > 1024) {
                    ready.call(img);
                    onReady.end = true;
                }
            };
            onReady();
            img.onload = function() { ! onReady.end && onReady();
                img = img.onload = img.onerror = null;
            };
        };
        util.resize = function(max, sou) {
            var sRate = sou.width / sou.height;
            if (max.width < sou.width) {
                sou.width = max.width;
                sou.height = sou.width / sRate;
            }
            if (max.height < sou.height) {
                sou.height = max.height;
                sou.width = sou.height * sRate;
            }
            return sou;
        };
        return util;
    });;
qcVideo('version',
    function() {
        var agent = navigator.userAgent;
        var v = {
            IOS: !!agent.match(/iP(od|hone|ad)/i),
            ANDROID: !!(/Android/i).test(agent)
        };
        var dom = document.createElement("video"),
            h5Able = {
                'probably': 1,
                'maybe': 1
            };
        dom = dom.canPlayType ? dom: null;
        v.IS_MAC = window.navigator && navigator.appVersion && navigator.appVersion.indexOf("Mac") > -1;
        v.ABLE_H5_MP4 = dom && (dom.canPlayType("video/mp4") in h5Able);
        v.ABLE_H5_WEBM = dom && (dom.canPlayType("video/webm") in h5Able);
        v.ABLE_H5_HLS = dom && (dom.canPlayType("application/x-mpegURL") in h5Able);
        v.IS_MOBILE = v.IOS || v.ANDROID;
        v.ABLE_H5_APPLE_HLS = dom && (dom.canPlayType("application/vnd.apple.mpegURL") in h5Able);
        v.FLASH_VERSION = -1;
        v.IS_IE = ("ActiveXObject" in window);
        v.ABLE_FLASH = (function() {
            var swf;
            if (document.all) try {
                swf = new ActiveXObject("ShockwaveFlash.ShockwaveFlash");
                if (swf) {
                    v.FLASH_VERSION = parseInt(swf.GetVariable("$version").split(" ")[1].split(",")[0]);
                    return ! 0;
                }
            } catch(e) {
                return ! 1;
            } else try {
                if (navigator.plugins && navigator.plugins.length > 0) {
                    swf = navigator.plugins["Shockwave Flash"];
                    if (swf) {
                        var words = swf.description.split(" ");
                        for (var i = 0; i < words.length; ++i) {
                            if (isNaN(parseInt(words[i]))) continue;
                            v.FLASH_VERSION = parseInt(words[i]);
                        }
                        return ! 0;
                    }
                }
            } catch(e) {
                return ! 1;
            }
            return ! 1;
        })();
        v.getFlashAble = function() {
            return v.ABLE_FLASH ? (v.FLASH_VERSION <= 10 ? 'lowVersion': 'able') : '';
        };
        var ableHlsJs = window.MediaSource && window.MediaSource.isTypeSupported('video/mp4; codecs="avc1.42E01E,mp4a.40.2"') ? true: false;
        var forceCheckHLS = function() {
            if (v.ANDROID && !v.ABLE_H5_HLS) {
                if (agent.substr(agent.indexOf('Android') + 8, 1) >= 4) {
                    return true;
                }
            }
            return false;
        };
        v.REQUIRE_HLS_JS = ableHlsJs && !v.ABLE_H5_HLS && !v.ABLE_H5_APPLE_HLS;
        v.getLivePriority = function() {
            if (v.IOS || v.ANDROID) {
                if (forceCheckHLS()) {
                    v.ABLE_H5_HLS = true;
                }
                return 'h5';
            }
            if (!v.ABLE_FLASH && v.ABLE_H5_MP4) {
                return 'h5';
            }
            return v.ABLE_FLASH ? 'flash': v.ABLE_H5_MP4 ? 'h5': '';
        };
        v.getVodPriority = function(inWhiteAppId) {
            if (v.IOS || v.ANDROID) {
                return 'h5';
            }
            if (!v.ABLE_FLASH && v.ABLE_H5_MP4) {
                return 'h5';
            }
            return v.ABLE_FLASH ? 'flash': v.ABLE_H5_MP4 ? 'h5': '';
        };
        v.PROTOCOL = (function() {
            try {
                var href = window.location.href;
                if (href.indexOf('https') === 0) {
                    return 'https';
                }
            } catch(xe) {}
            return 'http';
        })();
        return v;
    });;
qcVideo('config',
    function(version) {
        var vod = version.PROTOCOL + '://imgcache.qq.com/open/qcloud/video/h5';
        var live = version.PROTOCOL + '://imgcache.qq.com/open/qcloud/video/live';
        var flash = version.PROTOCOL + '://imgcache.qq.com/open/qcloud/video/flash';
        return {
            '$': vod + '/zepto-v1.1.3.min.js?max_age=20000000',
            'h5css': vod + '/video.css?ver=0531&max_age=20000000',
            'h5player': live + '/h5/h5_live_player.js',
            'flash': flash + '/live.swf?v=1224',
            set: function(key, url) {
                this[key] = url;
            }
        };
    });;
qcVideo('constants',
    function() {
        return {
            SERVER_API: "http://play.live.qcloud.com/",
            SERVER_API_HTTPS: "https://playlive.qcloud.com/",
            SERVER_API_PARAMS: {
                "channel_id": 1,
                "app_id": 1,
                "refer": 1,
                "passwd": true
            },
            OK_CODE: '0',
            ERROR_CODE: {
                TIME_OUT: '10000',
                REQUIRE_PWD: '11046',
                REQUIRE_APPID: '11044',
                REQUIRE_CHANNEL_ID: '11045',
                ILLEGAL_PWD: '20110',
                ILLEGAL_APPID: '10008',
                ILLEGAL_CHANNEL_ID: '10008',
                LIVE_NOT_EXSIT: '20113',
                ADDRESS_ILLEGAL: '1009',
                MANIFEST_ILL_VALID: '3',
                ARGUMENTS_ILL_VALID: '1001'
            },
            ERROR_MSG: {
                '10000': '\u8bf7\u6c42\u8d85\u65f6,\u8bf7\u68c0\u67e5\u7f51\u7edc\u8bbe\u7f6e',
                '11046': '\u5bc6\u7801\u9519\u8bef\uff0c\u8bf7\u91cd\u65b0\u8f93\u5165',
                '20110': '\u5bc6\u7801\u9519\u8bef\uff0c\u8bf7\u91cd\u65b0\u8f93\u5165',
                '20113': '\u76f4\u64ad\u6e90\u4e0d\u5b58\u5728(20113)',
                '1009': '\u76f4\u64ad\u6e90\u5df2\u5931\u6548(1009)',
                '3': '\u76f4\u64ad\u6e90\u5df2\u7ed3\u675f',
                '1001': '\u65e0\u6548\u53c2\u6570'
            },
            TAP: 'tap',
            CLICK: 'click',
            UNICODE_WORD: {
                TIP_REQUIRE_SAFARI: '\u5f53\u524d\u6d4f\u89c8\u5668\u4e0d\u80fd\u652f\u6301\u89c6\u9891\u64ad\u653e\u002c\u8bf7\u4f7f\u7528\u0073\u0061\u0066\u0061\u0072\u0069\u6253\u5f00\u89c2\u770b',
                TIP_REQUIRE_FLASH: '\u5f53\u524d\u6d4f\u89c8\u5668\u4e0d\u80fd\u652f\u6301\u89c6\u9891\u64ad\u653e\uff0c\u53ef\u4e0b\u8f7d\u6700\u65b0\u7684\u0051\u0051\u6d4f\u89c8\u5668\u6216\u8005\u5b89\u88c5\u0046\u004c\u0041\u0053\u0048\u5373\u53ef\u64ad\u653e'
            },
            HIGH_DEFINITION: 'high',
            NORMAL_DEFINITION: 'normal',
            ORIGINAL_DEFINITION: 'original',
            NAMES_DEFINITION: {
                "original": "超清",
                "high": "高清",
                "normal": "普清"
            }
        };
    });;
qcVideo('Barrage',
    function(Base, Barrage_tpl) {
        var uuid = 1000,
            addEvent = function(el, name, fn) {
                if (el.addEventListener) return el.addEventListener(name, fn, false);
                return el.attachEvent('on' + name, fn);
            },
            $ = function(id) {
                return document.getElementById(id);
            };
        return Base.extend({
            className: 'Barrage',
            option: null,
            constructor: function(targetId, context) {
                var id = targetId + '_' + (++uuid);
                var ids = {
                    "text": id + "text",
                    "send": id + "send"
                };
                var el = $(targetId);
                var div = document.createElement('div');
                div.className = 'trump-editor';
                div.innerHTML = Barrage_tpl.html({
                    ids: ids
                });
                el.parentNode.appendChild(div);
                var text = $(ids['text']);
                var send = $(ids['send']);
                var lastSendTime = +new Date();
                addEvent(send, 'click',
                    function() {
                        var now = +new Date();
                        if (now - lastSendTime > 3000) {
                            var value = text.value;
                            if (value) {
                                var bb = [{
                                    "type": "content",
                                    "content": value,
                                    "time": "0.101"
                                }];
                                context.addBarrage(bb);
                                lastSendTime = now;
                                text.value = '';
                            }
                        }
                    });
            }
        });
    });
qcVideo('Barrage_tpl',
    function() {
        return {
            'html': function(data) {
                var __p = [],
                    _p = function(s) {
                        __p.push(s)
                    };
                _p('<div class="trump-mask"></div>\r\n\
       <div class="trump-inner">\r\n\
           <div class="trump-textarea">\r\n\
               <textarea id="');
                _p(this.__escapeHtml(data.ids.text));
                _p('" class="trump-input" type="text" maxlength="50" placeholder="最多输入50个字符"></textarea>\r\n\
           </div>\r\n\
       </div>\r\n\
       <a id="');
                _p(this.__escapeHtml(data.ids.send));
                _p('" class="trump-submit" href="javascript:void;" onclick=";return false;">发&nbsp;送</a>\r\n\
       <style>\r\n\
           .trump-mask{display:none;width: 100%;height: 100%;position: absolute;z-index: 5;top: 0px;left: 0px;background-color: #000;opacity: 0.3;}\r\n\
           .trump-editor {width: 100%;background: #eee;position: relative;}\r\n\
           .trump-inner {padding: 10px;}\r\n\
           .trump-textarea {position: relative;background: #fff;border: #ccc solid 1px;height: 42px;}\r\n\
           .trump-input {border: 0;width: 80%;height: 32px;line-height: 22px;padding: 5px 10px;outline: none;overflow: hidden;color: #666;resize: none;background: transparent;}\r\n\
           .trump-submit {\r\n\
               position: absolute;right: 10px;top: 10px;width: 77px;height: 42px;line-height: 42px;text-decoration: none;\r\n\
               cursor: pointer;overflow: hidden;text-align: center;color: #fff;border: #e87e00 solid 1px;font-size: 14px;background-color: #ff5a00;\r\n\
               font-weight:bold;\r\n\
           }\r\n\
           .trump-submit:hover{background-color:#D6733D;}\r\n\
       </style>');
                return __p.join("");
            },
            __escapeHtml: (function() {
                var a = {
                        "&": "&amp;",
                        "<": "&lt;",
                        ">": "&gt;",
                        "'": "&#39;",
                        '"': "&quot;",
                        "/": "&#x2F;"
                    },
                    b = /[&<>'"\/]/g;
                return function(c) {
                    if (typeof c !== "string") return c;
                    return c ? c.replace(b,
                        function(b) {
                            return a[b] || b
                        }) : ""
                }
            })()
        }
    });;
qcVideo('H5',
    function(constants, api, util, Base, config, H5_tpl) {
        var verifyDone = function(data) {
            var me = this;
            util.merge(me.store, data, true);
            util.merge(me.store, {
                'parameter': me.option
            });
            me.loading(true);
            if (config.h5player.indexOf('?') === -1) {
                var version = data['version'] && data['version']['h5'] || 'beta';
                config.set('h5player', config.h5player + '?max_age=20000000&swfv=' + version);
            }
            qcVideo.use('h5player',
                function(mod) {
                    me.loading();
                    mod['render'](me.store);
                });
        };
        var $;
        return Base.extend({
            askDoor: function(firstTime, pass) {
                var me = this,
                    store = me.store,
                    key, address = ( !! me.store.https ? constants.SERVER_API_HTTPS: constants.SERVER_API) + '?t=' + ( + new Date());
                for (key in constants.SERVER_API_PARAMS) {
                    if (store.hasOwnProperty(key)) {
                        address += '&' + key + '=' + store[key];
                    }
                }
                if (pass !== undefined) {
                    address += '&passwd=' + pass;
                }
                if (me.option['live_url'] || me.option['live_url2']) {
                    var d = {};
                    d["channel_info"] = {};
                    var theUrl = me.option['live_url'].indexOf('.m3u8') > 0 ? me.option['live_url'] : me.option['live_url2'].indexOf('.m3u8') ? me.option['live_url2'] : '';
                    d["channel_info"]["hls_downstream_address"] = theUrl;
                    verifyDone.call(me, d);
                    return;
                }
                me.loading(true);
                api.request(address,
                    function(ret) {
                        me.loading();
                        var code = ret['retcode'] + '',
                            data = ret['data'];
                        if (code == constants.OK_CODE) {
                            if (!data.channel_info || !data.channel_info.hls_downstream_address) {
                                alert(constants.ERROR_MSG[constants.ERROR_CODE.LIVE_NOT_EXSIT + ''] + '(' + code + ') ');
                            } else {
                                verifyDone.call(me, data);
                            }
                        } else {
                            if ((code == constants.ERROR_CODE.REQUIRE_PWD || code == constants.ERROR_CODE.ILLEGAL_PWD) && firstTime) {
                                me.renderPWDPanel();
                            } else {
                                alert(constants.ERROR_MSG[code] || 'error code:(' + code + ') ');
                            }
                        }
                    });
            },
            className: 'PlayerH5',
            $pwd: null,
            $out: null,
            option: {},
            constructor: function(_$, targetId, opt) {
                $ = _$;
                var me = this,
                    node = document.createElement("link"),
                    defaultV = '20150508';
                me.option = opt;
                node.href = config.h5css;
                node.rel = "stylesheet";
                node.media = "screen";
                document.getElementsByTagName("head")[0].appendChild(node);
                me.store = util.merge({
                        "$renderTo": $('#' + targetId),
                        "version": {
                            "h5": defaultV,
                            "flash": defaultV,
                            "android": defaultV,
                            "ios": defaultV
                        }
                    },
                    opt);
                var $out = me.$out = me.store.$renderTo.html(H5_tpl['main']({
                    sure: '\u786e\u5b9a',
                    errpass: '\u62b1\u6b49\uff0c\u5bc6\u7801\u9519\u8bef',
                    enterpass: '\u8bf7\u8f93\u5165\u5bc6\u7801',
                    videlocked: '\u8be5\u89c6\u9891\u5df2\u52a0\u5bc6'
                }));
                $out.find('[data-area="main"]').css({
                    width: me.store.width,
                    height: me.store.height
                });
                me.$pwd = $out.find('[data-area="pwd"]');
                me.askDoor(true);
            },
            loading: function(visible) {},
            erTip: function(msg, pwdEr) {
                if (pwdEr) {
                    this.$pwd.find('.txt').text(msg).css('visibility', 'visible');
                }
            },
            sureHandler: function() {
                var me = this,
                    $pwd = me.$pwd,
                    pwd = $pwd.find('input[type="password"]').val() + '',
                    able = pwd.length > 0;
                $pwd.find('.txt').text(able ? '': '\u62b1\u6b49\uff0c\u5bc6\u7801\u9519\u8bef').css('visibility', (able ? 'hidden': 'visible'));
                if (able) {
                    me.askDoor(false, pwd);
                }
            },
            renderPWDPanel: function() {
                var me = this,
                    cw = me.store.width,
                    ch = me.store.height,
                    $pwd = me.$pwd,
                    $parent = $pwd.parent();
                $pwd.show().on('click', '[tx-act]',
                    function(e) {
                        var act = $(this).attr('tx-act'),
                            handler = me[act + 'Handler'];
                        handler && handler.call(me);
                        e.stopPropagation();
                        return false;
                    });
                var pw = $pwd.width(),
                    ph = $pwd.height(),
                    fW = $parent.width();
                if (fW && fW <= pw) {
                    $pwd.css({
                        'left': '0px',
                        'top': '0px'
                    }).width(fW);
                } else {
                    $pwd.css({
                        'left': (cw - pw) / 2 + 'px',
                        'top': (ch - ph) / 2 + 'px'
                    });
                }
            }
        });
    });
qcVideo('H5_tpl',
    function() {
        return {
            'main': function(data) {
                var __p = [],
                    _p = function(s) {
                        __p.push(s)
                    };
                _p('<div data-area="main" style="position: relative;background-color: #000;">\r\n\
           <div class="layer-password" data-area="pwd" style="display:none;">\r\n\
               <span class="tip" style="border: none;background-color: #242424;border-bottom: 1px solid #0073d0;position: relative;">');
                _p(data.videlocked);
                _p('</span>\r\n\
               <input class="password" placeholder="');
                _p(data.enterpass);
                _p('" type="password">\r\n\
               <span class="txt">');
                _p(data.errpass);
                _p('</span>\r\n\
               <div class="bottom">\r\n\
                   <a class="btn ok" href="#" tx-act="sure">');
                _p(data.sure);
                _p('</a>\r\n\
               </div>\r\n\
           </div>\r\n\
           <div data-area="loading" style="display:none;">\r\n\
               loading....\r\n\
           </div>\r\n\
    </div>');
                return __p.join("");
            },
            __escapeHtml: (function() {
                var a = {
                        "&": "&amp;",
                        "<": "&lt;",
                        ">": "&gt;",
                        "'": "&#39;",
                        '"': "&quot;",
                        "/": "&#x2F;"
                    },
                    b = /[&<>'"\/]/g;
                return function(c) {
                    if (typeof c !== "string") return c;
                    return c ? c.replace(b,
                        function(b) {
                            return a[b] || b
                        }) : ""
                }
            })()
        }
    });;
qcVideo('Player',
    function(util, Base, version, css, H5, Swf, SwfJsLink, constants) {
        var eidUuid = 10000000;
        function tryIt(fn) {
            fn();
        }
        function getEid() {
            return 'video_' + (eidUuid++);
        }
        function setSuitableWH(opt, ele) {
            tryIt(function() {
                ele.innerHTML = '';
            });
            var rate = 0,
                width = css.getWidth(ele),
                height = css.getHeight(ele),
                minPix = 4;
            if (width < minPix && ele.parentNode) {
                var pEle = ele.parentNode;
                while (true) {
                    if (!pEle || pEle === document.body) {
                        width = css.getVisibleWidth();
                        break;
                    } else {
                        width = css.getWidth(pEle);
                        if (width > minPix) {
                            break;
                        }
                    }
                    pEle = pEle.parentNode;
                }
            }
            var hasRate = opt.width > 0 && opt.height > 0;
            if (hasRate) {
                rate = opt.width / opt.height;
            }
            if (width > minPix && height < minPix && hasRate) {
                tryIt(function() {
                    ele.style.height = 'auto';
                });
                var vh = css.getVisibleHeight() - 4;
                var th = width / rate;
                if (vh === 0 || vh > th) {
                    height = th;
                } else {
                    height = vh;
                    width = rate * vh;
                }
                if (width > opt.width) {
                    width = opt.width;
                    height = width / rate;
                    if (height > opt.height) {
                        height = opt.height;
                        width = height * rate;
                    }
                } else if (height > opt.height) {
                    height = opt.height;
                    width = height * rate;
                    if (width > opt.width) {
                        width = opt.width;
                        height = height * rate;
                    }
                }
            }
            opt.width = width;
            opt.height = height;
        }
        return Base.extend({
            className: 'Player',
            constructor: function(targetId, opt, listener) {
                if (util.isPlainObject(targetId)) {
                    var tmp = opt;
                    opt = targetId;
                    targetId = tmp;
                }
                if (opt && opt['wording']) {
                    var i, m;
                    for (i in opt['wording']) {
                        m = opt['wording'][i];
                        if (constants.ERROR_MSG[i]) {
                            if (m) {
                                constants.ERROR_MSG[i] = m;
                            }
                        } else if (constants.UNICODE_WORD[i]) {
                            if (m) {
                                constants.UNICODE_WORD[i] = m;
                            }
                        }
                    }
                }
                var verifyDone = function() {
                    this.targetId = targetId;
                    setSuitableWH(opt, ele);
                    var ver = version.getLivePriority(),
                        eid = getEid();
                    var link = new SwfJsLink(eid, listener, targetId);
                    if (ver == 'h5' && (version.ABLE_H5_HLS || version.ABLE_H5_APPLE_HLS)) {
                        qcVideo.use('$',
                            function(mod) {
                                new H5(mod, targetId, opt);
                            });
                    } else if (ver == 'flash') {
                        new Swf(targetId, eid, opt, link);
                    } else {
                        if (version.IS_MAC) {
                            ele.innerText = constants.UNICODE_WORD.TIP_REQUIRE_SAFARI;
                        } else {
                            ele.innerText = constants.UNICODE_WORD.TIP_REQUIRE_FLASH;
                        }
                        return;
                    }
                    this.targetId = targetId;
                    css.textAlign(ele);
                    return link;
                };
                var ele = document.getElementById(targetId);
                opt['refer'] = document.domain;
                if (!targetId || !ele) {
                    alert("没有指定有效播放器容器！");
                } else if (!opt['live_url'] && (!opt['app_id'] || !opt['channel_id'])) {
                    alert("缺少参数，请补齐app_id，channel_id");
                } else {
                    return verifyDone.call(this);
                }
            },
            remove: function() {
                if (this.targetId) {
                    document.getElementById(this.targetId).innerHTML = '';
                }
            }
        });
    });;
qcVideo('Swf',
    function(Base, config) {
        var getHtmlCode = function(option, eid) {
            var __ = [],
                address = config.flash,
                _ = function(str) {
                    __.push(str);
                };
            var flashvars = (option.channel_id ? '&channel_id=' + option.channel_id: '') + (option.app_id ? '&app_id=' + option.app_id: '') + (option.https ? '&https=1': '') + (option.live_url ? '&live_url=' + encodeURIComponent(option.live_url) : '') + (option.live_url2 ? '&live_url2=' + encodeURIComponent(option.live_url2) : '') + (option.as3_trigger_core_event ? '&as3_trigger_core_event=' + option.as3_trigger_core_event: '') + '&refer=' + option.refer + '&jscbid=' + eid;
            var VMode = option.VMode || option.WMode || 'window';
            flashvars += !!option.debug ? '&debug=1': '';
            if (option.disable_full_screen !== undefined) {
                flashvars += '&disable_full_screen=' + option.disable_full_screen;
            }
            if (option.stretch_full !== undefined && !!option.stretch_full) {
                flashvars += '&stretch_full=1';
            }
            if (option.cache_time !== undefined) {
                flashvars += '&cache_time=' + option.cache_time;
            }
            if ( !! option.open_write_barrage) {
                flashvars += '&open_write_barrage=1';
            }
            _('<object data="' + address + '" id="' + eid + '_object" width="' + option.width + 'px" height="' + option.height + 'px"  style="background-color:#000000;" ');
            _('align="middle" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://fpdownload.macromedia.com/get/flashplayer/current/swflash.cab#version=9,0,0,0">');
            _('<param name="flashVars" value="' + flashvars + '"  />');
            _('<param name="src" value="' + address + '"  />');
            _('<param name="wmode" value="' + VMode + '"/>');
            _('<param name="quality" value="High"/>');
            _('<param name="allowScriptAccess" value="always"/>');
            _('<param name="allowNetworking" value="all"/>');
            _('<param name="allowFullScreen" value="true"/>');
            _('<embed style="background-color:#000000;"  id="' + eid + '_embed" width="' + option.width + 'px" height="' + option.height + 'px" flashvars="' + flashvars + '"');
            _('align="middle" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" allowfullscreen="true" bgcolor="#000000" quality="high"');
            _('src="' + address + '"');
            _('wmode="' + VMode + '" allowfullscreen="true" invokeurls="false" allownetworking="all" allowscriptaccess="always">');
            _('</object>');
            return __.join('');
        };
        return Base.extend({
            _recoverySize: function(old) {
                var width = old.width;
                var height = old.height;
                var me = this;
                if (width < 398 || height < 298) {
                    setTimeout(function() {
                            me.context.forceResize(width, height);
                        },
                        50);
                    old.width = 398;
                    old.height = 298;
                }
                return old;
            },
            className: 'PlayerSwf',
            option: null,
            constructor: function(targetId, eid, opt, context) {
                this.context = context;
                document.getElementById(this.targetId = targetId).innerHTML = getHtmlCode(this._recoverySize(opt), eid);
            },
            remove: function() {
                var node = document.getElementById(this.targetId) || {},
                    parent = node.parentNode;
                if (node.parentNode && (node.parentNode.tagName || '').toLowerCase() == 'object') {
                    node = parent;
                    parent = node.parentNode;
                }
                try {
                    parent.removeChild(node);
                } catch(xe) {}
            }
        });
    });;
qcVideo('SwfJsLink',
    function(util, JSON, LinkIm, Barrage) {
        var global = window;
        var pixesToInt = function(str) {
            return (str ? str + '': '').replace('px', '') | 0;
        };
        var SwfJsLink = function(id, listeners, targetId) {
            var me = this;
            me.id = id;
            me.tecGet = id + '_tecGet';
            me.barrage = id + '_barrage';
            me.close_barrage = id + '_close_barrage';
            me.login_barrage = id + '_login_barrage';
            me.check_login_barrage = id + '_check_login_barrage';
            me.__targetId = targetId;
            global[id + '_callback'] = function(cmd) {
                var cmds = cmd.split(':'),
                    key = cmds[0];
                if (me.listeners.hasOwnProperty(key)) {
                    switch (key) {
                        case('playStatus'):
                            me.listeners[key](cmds[1]);
                            break;
                    }
                }
            };
            global[id + '_call_js'] = function(cmd, data) {
                if (cmd == 'IM_LOGIN') {
                    var im = JSON.parse(data);
                    if (im && im['channel_id']) {
                        me.linkIm = new LinkIm(me, im, im['channel_id'],
                            function() {
                                me.listeners['loginBarrageCallback'](true);
                            },
                            function(msg) {
                                me.listeners['loginBarrageCallback'](false, msg);
                            });
                        if (im['open_write_barrage']) {
                            me.barrageObj = new Barrage(me.__targetId, me);
                        }
                    }
                }
            };
            me.listeners = {
                loginBarrageCallback: function(isDone, msg) {}
            };
            if (util.type(listeners) == 'object') {
                util.merge(me.listeners, listeners)
            }
        };
        var tryIt = function(fn) {
            return function() {
                try {
                    return fn.apply(this, arguments);
                } catch(xe) {
                    return '0';
                }
            };
        };
        util.merge(SwfJsLink.prototype, {
            getSwf: function() {
                var me = this;
                if (!me.swf) {
                    try {
                        var ctx1 = document.getElementById(this.id + '_object');
                        var ctx2 = document.getElementById(this.id + '_embed');
                        if (ctx1 && !!ctx1[this.tecGet]) {
                            this.swf = ctx1;
                        } else if (ctx2 && !!ctx2[this.tecGet]) {
                            this.swf = ctx2;
                        }
                    } catch(xe) {
                        return {};
                    }
                }
                return this.swf;
            },
            forceResize: function(w, h) {
                var r = [document.getElementById(this.id + '_object'), document.getElementById(this.id + '_embed')],
                    ctx,
                    i = 0;
                for (; i < 2; i++) {
                    try {
                        ctx = r[i];
                        if ( !! ctx) {
                            ctx.width = w;
                            ctx.height = h;
                        }
                    } catch(xe) {}
                }
            },
            resize: function(w, h) {
                var swf = this.getSwf();
                var numH = pixesToInt(h);
                if (swf) {
                    swf.width = pixesToInt(w);
                    swf.height = numH;
                }
            },
            getWidth: function() {
                return pixesToInt(this.getSwf().width);
            },
            getHeight: function() {
                return pixesToInt(this.getSwf().height);
            },
            addBarrage: tryIt(function(ary) {
                if (ary && ary.length > 0) {
                    return this.getSwf()[this.barrage](JSON.stringify(ary));
                }
            }),
            closeBarrage: tryIt(function() {
                return this.getSwf()[this.close_barrage]();
            }),
            loginBarrage: tryIt(function(info) {
                var m = info ? JSON.stringify(info) : '';
                return this.getSwf()[this.login_barrage](m);
            }),
            checkLoginBarrage: tryIt(function(appid) {
                return this.getSwf()[this.check_login_barrage](appid);
            })
        });
        return SwfJsLink;
    });
qcVideo.use("Player",
    function(mod) {
        qcVideo.Player = mod;
    });;
qcVideo('h5Drag',
    function($, Base) {
        return Base.extend({
            className: 'Drag',
            constructor: function(ctx, $drag, $range, onStart, onMove, onEnd) {
                var me = this,
                    getTouchLeft = function(e) {
                        return e.targetTouches[0].pageX
                    },
                    getRate = function() {
                        return ($drag.css('left').replace('px', '') - 0) / me.maxLeft;
                    };
                $(document.body).on('touchstart',
                    function(e) {
                        if (e.target === $drag.get(0) && ctx.enable_tag) {
                            var range = $range.offset();
                            me.sourceL = getTouchLeft(e);
                            me.maxLeft = range.width;
                            me.initLeft = me.maxLeft * ($drag.css('left').replace('%', '') - 0) / 100;
                            onStart && onStart();
                        } else {
                            me.sourceL = null;
                        }
                    }).on('touchmove',
                    function(e) {
                        if (me.sourceL !== null) {
                            var diff = getTouchLeft(e) - me.sourceL;
                            var newLeft = me.initLeft + diff;
                            if (newLeft >= me.maxLeft || newLeft <= 0) {
                                return;
                            }
                            $drag.css('left', newLeft + 'px');
                            onMove && onMove(getRate());
                        }
                    }).on('touchend',
                    function(e) {
                        if (me.sourceL !== null) {
                            onEnd && onEnd(getRate());
                        }
                    });
            }
        });
    });;
qcVideo('MediaPlayer',
    function($, Base, PlayerSystem, PlayerConst, MediaPlayer_tpl, constants) {
        return Base.extend({
            system: !1,
            control: !1,
            className: 'MediaPlayer',
            destroy: function() {
                if (this.control) {
                    this.control.destroy();
                    this.system.destroy();
                    delete this.control;
                    delete this.system;
                }
            },
            initVideo: function(video) {
                var me = this;
                if ( !! video) {
                    me.system.setUrl(video['url']);
                }
            },
            init_rates: function(video, channel_info) {
                if (!video.url || !channel_info.rate_type || channel_info.rate_type.length < 1) {
                    return;
                }
                var toRateUrls = function(type) {
                    var tail = (type == constants.ORIGINAL_DEFINITION) ? "": (type == constants.NORMAL_DEFINITION) ? "_550": "_900";
                    var rateName = constants.NAMES_DEFINITION[type];
                    var dot = video.url.indexOf('.m3u8');
                    return {
                        "url": video.url.substr(0, dot) + tail + video.url.substr(dot),
                        "name": rateName,
                        "rate": type
                    };
                };
                video.rate_type = {
                    __length: 0
                };
                video.rate_type.__length += 1;
                video.rate_type[constants.ORIGINAL_DEFINITION] = toRateUrls(constants.ORIGINAL_DEFINITION);
                var rate_type = channel_info['rate_type'],
                    type;
                for (var i = 0,
                         j = rate_type.length; i < j; i++) {
                    if (type = rate_type[i]) {
                        if (type == '10') {
                            video.rate_type.__length += 1;
                            video.rate_type[constants.NORMAL_DEFINITION] = toRateUrls(constants.NORMAL_DEFINITION);
                        } else if (type == '20') {
                            video.rate_type.__length += 1;
                            video.rate_type[constants.HIGH_DEFINITION] = toRateUrls(constants.HIGH_DEFINITION);
                        }
                    }
                }
            },
            constructor: function(setting) {
                var me = this,
                    $renderTo = setting.$renderTo,
                    video = {
                        url: setting.channel_info['hls_downstream_address']
                    },
                    $container = $renderTo.html(MediaPlayer_tpl['main']({
                        width: setting.width,
                        height: setting.height
                    })).find('div'),
                    system = me.system = new PlayerSystem($container);
                if (video.url) {
                    me.init_rates(video, setting.channel_info);
                    if (video.rate_type && video.rate_type.high) {
                        if (video.url.indexOf('?') < 1) {
                            video.url = video.rate_type.high.url;
                        }
                    }
                }
                var width = $renderTo.width() - 4;
                me.initVideo(video);
                $container.append(MediaPlayer_tpl['controller']({
                    "width": parseInt(width / 7),
                    "height": parseInt(width / 7),
                    "start_patch": setting['h5_start_patch']
                }));
                $container.css('overflow', 'hidden');
                var $play = $renderTo.find('[sub-component="play"]');
                var $video = $renderTo.find('video');
                var $startPatch = $renderTo.find('[ data-mode="start_patch"]');
                var callPlayed = false;
                var toPlay = function() {
                    callPlayed = true;
                    $play.hide();
                    $startPatch.hide();
                    $video.css('top', '0');
                    system.play();
                };
                $play.on('click',
                    function() {
                        toPlay();
                    });
                if (setting.h5_on_touch_play) {
                    $(document).one("touchstart",
                        function() {
                            if (!callPlayed) {
                                toPlay();
                            }
                        });
                }
            }
        });
    });
qcVideo('MediaPlayer_tpl',
    function() {
        return {
            'main': function(data) {
                var __p = [],
                    _p = function(s) {
                        __p.push(s)
                    };
                _p('<div style="width:');
                _p(data.width);
                _p('px;height:');
                _p(data.height);
                _p('px;margin: 0px auto;position:relative;background-color: #000;"></div>');
                return __p.join("");
            },
            'controller': function(data) {
                var __p = [],
                    _p = function(s) {
                        __p.push(s)
                    };
                var patch = data.start_patch;
                if (patch) {
                    if (patch.stretch) {
                        _p('            <img data-mode="start_patch" style="width:100%;height:100%" src="');
                        _p(this.__escapeHtml(patch.url));
                        _p('"/>');
                    } else {
                        _p('            <img data-mode="start_patch" src="');
                        _p(this.__escapeHtml(patch.url));
                        _p('"/>');
                    }
                }
                _p('    <div style="position: absolute;left: 50%;top: 50%;z-index: 101;cursor: pointer;width: ');
                _p(this.__escapeHtml(data.width));
                _p('px;height:');
                _p(this.__escapeHtml(data.height));
                _p('px;margin: -');
                _p(this.__escapeHtml(data.width / 2));
                _p('px 0 0 -');
                _p(this.__escapeHtml(data.height / 2));
                _p('px;" sub-component="play">\r\n\
           <svg height="100%" version="1.1" viewBox="0 0 98 98" width="100%">\r\n\
               <circle cx="49" cy="49" fill="#000" stroke="#fff" stroke-width="2" fill-opacity="0.5" r="48" data-opacity="true"></circle>\r\n\
               <circle cx="-49" cy="49" fill-opacity="0" r="46.5" stroke="#fff"\r\n\
                       stroke-dasharray="293" stroke-dashoffset="-293.0008789998712" stroke-width="4"\r\n\
                       transform="rotate(-90)"></circle>\r\n\
               <polygon fill="#fff" points="32,27 72,49 32,71"></polygon>\r\n\
           </svg>\r\n\
       </div>');
                return __p.join("");
            },
            'definition_panel': function(data) {
                var __p = [],
                    _p = function(s) {
                        __p.push(s)
                    };
                _p('<ul class="distinct">');
                for (var i = 0,
                         j = data.length; i < j; i++) {
                    var itm = data[i];
                    _p('                <li resolution="');
                    _p(itm.resolution);
                    _p('"><a href="#">');
                    _p(itm.resolutionName);
                    _p('</a></li>');
                }
                _p('    </ul>');
                return __p.join("");
            },
            'video': function(data) {
                var __p = [],
                    _p = function(s) {
                        __p.push(s)
                    };
                _p('<video id="');
                _p(data.vid);
                _p('" webkit-playsinline="" width="100%" height="100%" controls=\'true\' src="');
                _p(data.url);
                _p('" style="z-index: 1;position: absolute;top: -200%;left: 0px;"></video>');
                return __p.join("");
            },
            __escapeHtml: (function() {
                var a = {
                        "&": "&amp;",
                        "<": "&lt;",
                        ">": "&gt;",
                        "'": "&#39;",
                        '"': "&quot;",
                        "/": "&#x2F;"
                    },
                    b = /[&<>'"\/]/g;
                return function(c) {
                    if (typeof c !== "string") return c;
                    return c ? c.replace(b,
                        function(b) {
                            return a[b] || b
                        }) : ""
                }
            })()
        }
    });;
qcVideo('PlayerConst',
    function() {
        return {
            EVENT: {
                OS_TIME_UPDATE: 'OS_TIME_UPDATE',
                OS_PROGRESS: 'OS_PROGRESS',
                OS_LOADED_META_DATA: 'OS_LOADED_META_DATA',
                OS_PLAYER_END: 'OS_PLAYER_END',
                OS_VIDEO_LOADING: 'OS_VIDEO_LOADING',
                OS_ERROR: 'OS_ERROR',
                OS_BLOCK: 'OS_BLOCK',
                UI_SET_VOLUME: 'UI_SET_VOLUME',
                UI_SEEK_TIME: 'UI_SEEK_TIME',
                UI_SWITCH_DEFINITION: 'UI_SWITCH_DEFINITION',
                UI_PAUSE: 'UI_PAUSE',
                UI_PLAY: 'UI_PLAY',
                UI_CLICK_DEFINITION: 'UI_CLICK_DEFINITION',
                UI_BACK: 'UI_BACK',
                UI_SETTING: 'UI_SETTING',
                UI_FULL_SCREEN: 'UI_FULL_SCREEN'
            },
            ERROR: {
                DISABLE_VISIT: '当前视频无法访问'
            }
        };
    });;
qcVideo('PlayerStatus',
    function(Base, PlayerConst, interval) {
        var time = 1000 / 60,
            blockTime = time * 300,
            EVENT = PlayerConst.EVENT;
        return Base.extend({
            className: 'PlayerStatus',
            clear: function() {
                var me = this;
                me.played = 0;
                me.duration = 0;
                me.loaded = 0;
                me.loaded_overtime = 0;
                me.errorCode = 0;
                me.equalRead();
                me.timeTask.start();
            },
            equalRead: function() {
                var me = this;
                me.__read = {
                    played: me.played,
                    duration: me.duration,
                    loaded: me.loaded
                };
            },
            constructor: function() {
                var me = this;
                me.timeTask = interval(function() {
                        var bool = false;
                        if (me.__read.duration !== me.duration) {
                            bool = true;
                            me.fire(EVENT.OS_LOADED_META_DATA);
                        }
                        if (me.__read.played !== me.played) {
                            bool = true;
                            me.fire(EVENT.OS_PROGRESS);
                        }
                        if (me.__read.loaded !== me.loaded) {
                            bool = true;
                            me.fire(EVENT.OS_TIME_UPDATE);
                        }
                        me.equalRead();
                        if (!bool && me.__isMaybeBlockStatus()) {
                            if (me.played >= me.duration && me.duration > 0) {
                                me.fire(EVENT.OS_PLAYER_END);
                            }
                            var now = +new Date();
                            if (now - me.status_start > blockTime && me.__getStatusValue() === me.status_value) {
                                me.fire(EVENT.OS_BLOCK);
                            } else {
                                me.fire(EVENT.OS_VIDEO_LOADING);
                            }
                        }
                    },
                    time);
                me.clear();
            },
            destroy: function() {
                this.timeTask.clear();
            },
            __getStatusValue: function() {
                return (this.played - 0) + ':' + (this.loaded - 0) + ':' + (this.duration - 0);
            },
            __isMaybeBlockStatus: function() {
                return this.status === 'play' || this.status === 'load';
            },
            setRunningStatus: function(status) {
                this.status = status;
                this.status_start = +new Date();
                this.status_value = this.__getStatusValue();
            },
            set_duration: function(num) {
                this.duration = num - 0;
            },
            set_loaded: function(num) {
                this.loaded = num - 0;
            },
            set_played: function(num) {
                this.played = num - 0;
            }
        });
    });;
qcVideo('PlayerSystem',
    function($, Base, PlayerStatus, MediaPlayer_tpl, interval) {
        var getId = function() {
                return 'video_id_' + ( + new Date());
            },
            time = 1000 / 60;
        return Base.extend({
            className: 'PlayerSystem',
            constructor: function($renderTo) {
                var me = this;
                me.$renderTo = $renderTo;
                me.status = new PlayerStatus();
                me.timeTask = interval(function() {
                        if (me.video) {
                            var r = me.video.buffered,
                                loaded = 0;
                            if (r) {
                                for (var i = 0; i < r.length; i++) {
                                    loaded = r.end(i) - 0;
                                }
                            }
                            me.status.set_loaded(loaded);
                            me.status.set_played(me.video.currentTime);
                        }
                    },
                    time);
            },
            destroy: function() {
                this.timeTask.clear();
                delete this.$renderTo;
                this.status.destroy();
                delete this.status;
            },
            getStatus: function() {
                return this.status;
            },
            callMethod: function(mtd) {
                try {
                    this.video[mtd]();
                    this.status.setRunningStatus(mtd);
                } catch(xe) {}
            },
            _bind: function() {
                var me = this;
                var getHandler = function(event) {
                    return function(e) {
                        var video = me.video;
                        switch (event) {
                            case('loadedmetadata'):
                                me.metadatadone = true;
                                me.status.set_duration(video.duration);
                                me.timeTask.start();
                                break;
                            case ('error'):
                                me.log(event, e);
                                break;
                        }
                    };
                };
                $.each('loadedmetadata,error'.split(','),
                    function(_, event) {
                        me.$video.on(event, getHandler(event));
                    });
            },
            setUrl: function(src) {
                var me = this,
                    $renderTo = me.$renderTo,
                    tpl = me.tpl = {
                        vid: getId(),
                        width: $renderTo.width(),
                        height: $renderTo.height(),
                        url: src
                    };
                me.metadatadone = false;
                me.timeTask.pause();
                me.status.clear();
                me.$video && me.$video.remove();
                $renderTo.prepend(MediaPlayer_tpl['video'](tpl));
                me.video = (me.$video = $('#' + tpl.vid)).get(0);
                me._bind();
            },
            play: function() {
                var me = this;
                if (me.video) {
                    me.callMethod('play');
                }
            },
            setVolume: function(num) {
                if (this.video) {
                    this.video.volume = num;
                }
            }
        });
    });;
qcVideo('h5player',
    function($, Base, constants, util, MediaPlayer) {
        return Base.instance({
            className: 'h5player',
            constructor: Base.loop,
            render: function(opt) {
                this.mediaPlayer = new MediaPlayer({
                    width: opt.width,
                    height: opt.height,
                    $renderTo: opt.$renderTo,
                    channel_info: opt.channel_info,
                    player_info: opt.player_info,
                    h5_start_patch: opt.h5_start_patch || '',
                    h5_on_touch_play: opt.h5_on_touch_play
                });
            },
            destroy: function() {
                this.mediaPlayer.destroy();
            }
        });
    });
/*  |xGv00|7b6501deb85d975889189ea7a7516410 */

