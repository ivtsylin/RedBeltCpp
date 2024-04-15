#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>
#include <memory>
#include <optional>

namespace Svg {

struct Point {
    double x;
    double y;
};

struct Rgb {
    int red;
    int green;
    int blue;
    bool operator==(const Rgb& rhs) const {
        return std::tie(red, green, blue) == std::tie(rhs.red, rhs.green, rhs.blue);
    }
};

struct Rgba {
    int red;
    int green;
    int blue;
    double alpha;
    bool operator==(const Rgba& rhs) const {
        return std::tie(red, green, blue, alpha) == std::tie(rhs.red, rhs.green, rhs.blue, rhs.alpha);
    }
};

std::ostream& operator<< (std::ostream& sout, const Rgb& rhs);

class Color {
public:
    class NoValue{
    public:
        bool operator==(const Svg::Color::NoValue& rhs) const {
            return true;
        }
    };
    
    Color() : color(NoValue{}) {};
    template<class T>
    Color(const T &col) : color(col) {};
    
    bool operator==(const Svg::Color& rhs) const {
        return color == rhs.color;
    }
    
    friend std::ostream& operator<< (std::ostream& sout, const Color& rhs);
    
    ~Color() {};
    
private:
    std::variant<NoValue, std::string, Rgb, Rgba> color;
};

static const Color NoneColor{};

std::ostream& operator<< (std::ostream& sout, const Color::NoValue& rhs);
std::ostream& operator<< (std::ostream& sout, const Color& rhs);

template<class T>
class GraphPrimitive {
private:
    Color fill;
    Color stroke;
    double stroke_width;
    std::optional<std::string> stroke_linecap;
    std::optional<std::string> stroke_linejoin;
public:
    GraphPrimitive() : fill(), stroke(), stroke_width(1.0) {};
    
    T& SetFillColor(const Color& fill_ = NoneColor) {
        fill = fill_;
        return static_cast<T&>(*this);
    }
    
    T& SetStrokeColor(const Color& stroke_ = NoneColor) {
        stroke = stroke_;
        return static_cast<T&>(*this);
    }
    
    T& SetStrokeWidth(double stroke_width_ = 1.0) {
        stroke_width = stroke_width_;
        return static_cast<T&>(*this);
    }
    
    T& SetStrokeLineCap(const std::string& stroke_linecap_) {
        stroke_linecap = stroke_linecap_;
        return static_cast<T&>(*this);
    }
    
    T& SetStrokeLineJoin(const std::string& stroke_linejoin_) {
        stroke_linejoin = stroke_linejoin_;
        return static_cast<T&>(*this);
    }
    
    void Render(std::ostream& sout) const {
        sout << "fill=\\\"" << fill << "\\\" stroke=\\\"" << stroke << "\\\" stroke-width=\\\"" << stroke_width << "\\\" ";
        if (stroke_linecap) {
            sout << "stroke-linecap=\\\"" << stroke_linecap.value() << "\\\" ";
        }
        if (stroke_linejoin) {
            sout << "stroke-linejoin=\\\"" << stroke_linejoin.value() << "\\\" ";
        }
    }
};

class Circle : public GraphPrimitive<Circle> {
private:
    Point center;
    double r;
public:
    Circle() : GraphPrimitive::GraphPrimitive(), center({0.0, 0.0}), r(1.0) {};
    
    Circle& SetCenter(Point p) {
        center = p;
        return *this;
    };
    
    Circle& SetRadius(double r_) {
        r = r_;
        return *this;
    };
    
    void Render(std::ostream& sout) const {
        sout << "<circle cx=\\\"" << center.x << "\\\" cy=\\\"" << center.y << "\\\" r=\\\"" << r << "\\\" ";
        GraphPrimitive::Render(sout);
        sout << "/>";
    }
};

class Polyline : public GraphPrimitive<Polyline> {
private:
    std::vector<Point> points;
public:
    
    Polyline() : GraphPrimitive::GraphPrimitive() {};
    
    Polyline& AddPoint(Point p) {
        points.push_back(p);
        return *this;
    }
    
    void Render(std::ostream& sout) const {
        sout << "<polyline points=\\\"";
        for (const auto& p : points) {
            sout << p.x << "," << p.y << " ";
        }
        sout << "\\\" ";
        GraphPrimitive::Render(sout);
        sout << "/>";
    }
};

class Text : public GraphPrimitive<Text> {
private:
    Point p;
    Point dp;
    uint32_t font_size;
    std::optional<std::string> font_family;
    std::string text;
    std::optional<std::string> font_weight;
    
public:
    
    Text() : GraphPrimitive::GraphPrimitive(), p({0.0, 0.0}), dp({0.0, 0.0}), font_size(1) {};
    
    Text& SetPoint(Point p_) {
        p = p_;
        return *this;
    }
    
    Text& SetOffset(Point dp_) {
        dp = dp_;
        return *this;
    }
    
    Text& SetFontSize(uint32_t font_size_) {
        font_size = font_size_;
        return *this;
    }
    
    Text& SetFontFamily(const std::string& font_family_) {
        font_family = font_family_;
        return *this;
    }
    
    Text& SetData(const std::string& text_) {
        text = text_;
        return *this;
    }
    
    Text& SetFontWeight(const std::string& font_weight_) {
        font_weight = font_weight_;
        return *this;
    }
    
    void Render(std::ostream& sout) const {
        sout << "<text x=\\\"" << p.x << "\\\" y=\\\"" << p.y << "\\\" dx=\\\"" << dp.x << "\\\" dy=\\\"" << dp.y << "\\\" font-size=\\\"" << font_size << "\\\" ";
        if (font_family) {
            sout << "font-family=\\\"" << font_family.value() << "\\\" ";
        }
        if (font_weight) {
            sout << "font-weight=\\\"" << font_weight.value() << "\\\" ";
        }
        GraphPrimitive::Render(sout);
        sout << ">" << text << "</text>";
    }
};


}
