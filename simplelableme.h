#ifndef SIMPLELABLEME_H
#define SIMPLELABLEME_H

#include <QMainWindow>
#include <QLabel>

#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardItemModel>
#include <QDomDocument>
#include "logger_class.h"

namespace Ui {
class SimpleLableMe;
}

class SimpleLableMe : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimpleLableMe(QWidget *parent = 0);
    ~SimpleLableMe();

    void Load_img_on_Label(QLabel* customLabel,QString fileName,bool b_resize,QWidget* parentWidget);
    void Load_img_on_Label(QLabel* customLabel,cv::Mat img,bool b_resize,QWidget* parentWidget);

    void draw_circle(int x, int y, int radius, QLabel *label);
    void draw_circle(QPoint center, float radius, QLabel *label);
    void draw_line(QPoint p_start, QPoint p_end, QLabel *label);

    void init_Point_Tableview();

    void write_xml(QString filename);
    void read_xml_and_show_points(QString filename);

    void restore_last_setting();

    void find_existing_xml();

    QImage Mat2QImage(cv::Mat const& src);
    cv::Mat QImage2Mat(QImage const& src);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;


private slots:
    void on_pushButton_OpenDir_clicked();

    void on_pushButton_SaveDir_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

    void on_pushButton_save_clicked();

    void on_pushButton_Next_clicked();

    void on_pushButton_Previous_clicked();

    void on_pushButton_Create_Polygon_clicked();

    void on_tableView_points_doubleClicked(const QModelIndex &index);

    void on_pushButton_show_contour_clicked();

    void on_pushButton_MoveUp_clicked();

    void on_pushButton_Down_clicked();

    void on_pushButton_del_clicked();

    void on_listView_indexesMoved(const QModelIndexList &indexes);

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::SimpleLableMe *ui;

    QPoint mousePressPosition;
    QPoint next_mousePressPosition;

    std::vector<QPoint> m_vector_line_start;
    std::vector<QPoint> m_vector_line_end;

    std::vector<QRect> m_vector_rectangle;

    std::vector<QPoint> m_vector_circle_center;
    std::vector<float> m_vector_circle_radius_x;

    std::vector<float> m_vector_circle_radius_y;

    int m_index=0;
    int m_list_index=0;

    QStandardItemModel *image_points_model;

    QString m_current_filename;
    QString m_current_filename_relative;
    QString m_current_filename_path;

    QString m_xml_save_dir;
    QString m_image_dir;

    bool m_draw_poly_en=false;
    cv::Mat m_img;

    int m_img_width;
    int m_img_height;

    Logger_Class log;//日志类
    QString m_log_filename;


};

#endif // SIMPLELABLEME_H
