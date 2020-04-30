#include "simplelableme.h"
#include "ui_simplelableme.h"
#include <QFileDialog>
#include <QDebug>
#include <QStringListModel>
#include <QSettings>
#include <QLabel>
#include <QXmlStreamReader>
SimpleLableMe::SimpleLableMe(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SimpleLableMe)
{

    QDateTime dateTime = QDateTime::currentDateTime();

    m_log_filename="zhanjiang_"+dateTime.toString("yyyyMMddhhmmss")+".txt";
    log.init_log(m_log_filename);
    qInstallMessageHandler((log.outputMessage));


    ui->setupUi(this);
    ui->label_img->setScaledContents(true);
    init_Point_Tableview();
    restore_last_setting();
}

SimpleLableMe::~SimpleLableMe()
{
    delete ui;
}
void SimpleLableMe::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
//   mousePressPosition = QVector2D(e->localPos());
   QPoint leftButtomRightPoint = e->globalPos();
   //获取全局位置
   mousePressPosition = ui->label_img->mapFromGlobal(leftButtomRightPoint);
   int i=0;
   qDebug()<<mousePressPosition.rx()<<endl;
   qDebug()<<mousePressPosition.ry()<<endl;


}

void SimpleLableMe::mouseReleaseEvent(QMouseEvent *e)
{
    // Save mouse press position
//   mousePressPosition = QVector2D(e->localPos());
   QPoint leftButtomRightPoint = e->globalPos();
   //获取全局位置
   mousePressPosition = ui->label_img->mapFromGlobal(leftButtomRightPoint);
   int i=0;

   if(m_draw_poly_en)
       draw_circle(mousePressPosition, 5, ui->label_img);





}

void SimpleLableMe::on_pushButton_OpenDir_clicked()
{
    QString fileName=QFileDialog::getExistingDirectory(this,tr("OpenDirectory"),m_image_dir,
    QFileDialog::ShowDirsOnly
    |QFileDialog::DontResolveSymlinks);


    QDir dir(fileName);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png"<< "*.bmp";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    for(int i=0;i<files.size();i++)
        qDebug()<<files.at(i)<<endl;

    QStringListModel *model = new QStringListModel(files);
    ui->listView->setModel(model);


    QSettings settings("SimpleLabelMe", "soft_v1");

    settings.setValue("images_dir",fileName);



}

void SimpleLableMe::on_pushButton_SaveDir_clicked()
{
    QString fileName=QFileDialog::getExistingDirectory(this,tr("OpenDirectory"),m_xml_save_dir,
    QFileDialog::ShowDirsOnly
    |QFileDialog::DontResolveSymlinks);
    QSettings settings("SimpleLabelMe", "soft_v1");
    settings.setValue("save_labelresult_dir",fileName);
    m_xml_save_dir=fileName;

    QString s1=settings.value("save_labelresult_dir").toString();
    int i=0;

}

void SimpleLableMe::on_listView_doubleClicked(const QModelIndex &index)
{

    m_index=0;
    m_list_index=index.row();

    QSettings settings("SimpleLabelMe", "soft_v1");

    QString file_dir=settings.value("images_dir").toString();

    QString str=ui->listView->model()->data(index).toString();
    qDebug()<<str<<endl;

    QString filename=file_dir+"/"+str;
    m_current_filename_relative=str;
    m_current_filename_path=file_dir;
    Load_img_on_Label(ui->label_img,filename,true,ui->widget_img);
    m_img=cv::imread(filename.toStdString());
    cv::waitKey(10);

    find_existing_xml();

}

void SimpleLableMe::Load_img_on_Label(QLabel* customLabel,QString fileName,bool b_resize,QWidget* parentWidget)
{
    m_current_filename=fileName;
    QImage img;
    if(!(img.load(fileName)))//加载图像
    {
        QMessageBox::information(this,
                                 tr("fail open image file."),
                                 tr("fail!"));

        return;
    }

    m_img_width=img.width();
    m_img_height=img.height();
    customLabel->setPixmap(QPixmap::fromImage(img));
    if(b_resize)
    {
//        customLabel->setScaledContents(true);
        customLabel->resize(parentWidget->size());
    }

}
void SimpleLableMe::Load_img_on_Label(QLabel* customLabel,cv::Mat img,bool b_resize,QWidget* parentWidget)
{
    QImage q_img=Mat2QImage(img);
    m_img_width=q_img.width();
    m_img_height=q_img.height();
    customLabel->setPixmap(QPixmap::fromImage(q_img));
    if(b_resize)
    {
//        customLabel->setScaledContents(true);
        customLabel->resize(parentWidget->size());
    }

}
void SimpleLableMe::draw_line(QPoint p_start, QPoint p_end, QLabel *label)
{
    QImage tmp(label->pixmap()->toImage());
    int width=tmp.width();
    int height=tmp.height();
    int label_width=label->size().width();
    int label_height=label->size().height();
    float scale_x=1.0*width/label_width;
    float scale_y=1.0*height/label_height;
    QPainter painter(&tmp);
    QPen paintpen(Qt::red);
    paintpen.setWidth(10);
//    QPoint p1;
//    p1.setX(p_start);
//    p1.setY(p_end);
    painter.setPen(paintpen);
//    painter.drawPoint(p1);
    painter.drawLine(p_start.x()*scale_x,p_start.y()*scale_y,p_end.x()*scale_x,p_end.y()*scale_y);
    painter.end();
    label->setPixmap(QPixmap::fromImage(tmp));
    label->resize(ui->widget_img->size());

    QPoint new_p_start,new_p_end;
    new_p_start.setX(p_start.x()*scale_x);
    new_p_start.setY(p_start.y()*scale_y);
    new_p_end.setX(p_end.x()*scale_x);
    new_p_end.setY(p_end.y()*scale_y);


    m_vector_line_start.push_back(new_p_start);
    m_vector_line_end.push_back(new_p_end);

    cv::Point p1,p2;
    p1.x=p_start.x()*scale_x;
    p1.y=(p_start.y())*scale_y;
    p2.x=p_end.x()*scale_x;
    p2.y=(p_end.y())*scale_y;
//    cv::line(img_for_segment,p1,p2,cv::Scalar(255,255,255),10);
//    cv::imshow("background",img_for_segment);

}
void SimpleLableMe::draw_circle(QPoint center, float radius, QLabel *label)
{
    QImage tmp(label->pixmap()->toImage());
    int width=tmp.width();
    int height=tmp.height();
    int label_width=label->size().width();
    int label_height=label->size().height();
    float scale_x=1.0*width/label_width;
    float scale_y=1.0*height/label_height;
    QPainter painter(&tmp);
    QPen paintpen(Qt::red);
    paintpen.setWidth(10);
    painter.setPen(paintpen);
//    painter.drawPoint(p1);
    QPoint new_center;
    new_center.setX(center.x()*scale_x);
    new_center.setY(center.y()*scale_y);
    painter.drawEllipse(new_center,(int)(radius*scale_x),(int)(radius*scale_y));
    painter.end();

    image_points_model->setItem(m_index,0,new QStandardItem(QString::number((int)(center.x()*scale_x))));
    image_points_model->setItem(m_index,1,new QStandardItem(QString::number((int)(center.y()*scale_y))));
    m_index++;


    label->setPixmap(QPixmap::fromImage(tmp));
    label->resize(ui->widget_img->size());
    cv::Point cv_center;
    int radius_x,radius_y;
    cv_center.x=new_center.x();
    cv_center.y=new_center.y();
    radius_x=radius*scale_x;
    radius_y=radius*scale_y;
//    cv::ellipse(img_for_segment,cv_center,cv::Size(radius_x,radius_y),0,0,360,cv::Scalar(255,255,255),10);
//    imshow("background",img_for_segment);

}
void SimpleLableMe::draw_circle(int x,int y,int radius, QLabel *label)
{
    QImage tmp(label->pixmap()->toImage());
    int width=tmp.width();
    int height=tmp.height();
    int label_width=label->size().width();
    int label_height=label->size().height();
    float scale_x=1.0*width/label_width;
    float scale_y=1.0*height/label_height;
    QPainter painter(&tmp);
    QPen paintpen(Qt::red);
    paintpen.setWidth(10);
    painter.setPen(paintpen);
//    painter.drawPoint(p1);
    QPoint new_center;
    new_center.setX(x);
    new_center.setY(y);
    painter.drawEllipse(new_center,(int)(radius),(int)(radius));
    painter.end();

//    image_points_model->setItem(m_index,0,new QStandardItem(QString::number((int)(center.x()*scale_x))));
//    image_points_model->setItem(m_index,1,new QStandardItem(QString::number((int)(center.y()*scale_y))));
//    m_index++;


    label->setPixmap(QPixmap::fromImage(tmp));
    label->resize(ui->widget_img->size());
//    cv::Point cv_center;
//    int radius_x,radius_y;
//    cv_center.x=new_center.x();
//    cv_center.y=new_center.y();
//    radius_x=radius*scale_x;
//    radius_y=radius*scale_y;
//    cv::ellipse(img_for_segment,cv_center,cv::Size(radius_x,radius_y),0,0,360,cv::Scalar(255,255,255),10);
//    imshow("background",img_for_segment);

}
void SimpleLableMe::init_Point_Tableview()
{
    image_points_model = new QStandardItemModel();
    ui->tableView_points->setModel(image_points_model);
    image_points_model->setHorizontalHeaderItem(0, new QStandardItem(tr("x")));
    image_points_model->setHorizontalHeaderItem(1, new QStandardItem(tr("y")));

    ui->tableView_points->setColumnWidth(0,150);
    ui->tableView_points->setColumnWidth(1,150);
}

void SimpleLableMe::write_xml(QString filename)
{
    QFile file(filename); //相对路径、绝对路径、资源路径都可以

    if(!file.open(QFile::WriteOnly|QFile::Truncate)) //可以用QIODevice，Truncate表示清空原来的内容

        return;



    QDomDocument doc;

    //写入xml头部

    QDomProcessingInstruction instruction; //添加处理命令

    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");

    doc.appendChild(instruction);

    //添加根节点

    QDomElement root=doc.createElement("annotation");

    doc.appendChild(root);

    QDomElement folder=doc.createElement("folder");
    QDomText text=doc.createTextNode(m_current_filename_path);
    folder.appendChild(text);
    root.appendChild(folder);

    QDomElement filename_elem=doc.createElement("filename");
    text=doc.createTextNode(m_current_filename_relative);
    filename_elem.appendChild(text);
    root.appendChild(filename_elem);

    QDomElement path=doc.createElement("path");
    text=doc.createTextNode(m_current_filename_path+"/"+m_current_filename_relative);
    path.appendChild(text);
    root.appendChild(path);

    QDomElement source=doc.createElement("source");
    QDomElement database=doc.createElement("database");
    text=doc.createTextNode("database");
    database.appendChild(text);
    source.appendChild(database);
    root.appendChild(source);

    QDomElement size=doc.createElement("size");
    QDomElement width=doc.createElement("width");
    text=doc.createTextNode(QString::number(m_img_width));
    width.appendChild(text);
    QDomElement height=doc.createElement("height");
    text=doc.createTextNode(QString::number(m_img_height));
    height.appendChild(text);
    QDomElement depth=doc.createElement("depth");
    text=doc.createTextNode("depth");
    depth.appendChild(text);

    size.appendChild(width);
    size.appendChild(height);
    size.appendChild(depth);

    root.appendChild(size);

    QDomElement segment=doc.createElement("segment");
    text=doc.createTextNode("segment");
    segment.appendChild(text);
    root.appendChild(segment);


    QDomElement object_point=doc.createElement("object");
    QDomElement name=doc.createElement("name");
    text=doc.createTextNode("contour points");
    name.appendChild(text);
    QDomElement pose=doc.createElement("pose");
    text=doc.createTextNode("pose");
    pose.appendChild(text);
    QDomElement truncated=doc.createElement("truncated");
    text=doc.createTextNode("truncated");
    truncated.appendChild(text);
    QDomElement diffcult=doc.createElement("difficult");
    text=doc.createTextNode("difficult");
    diffcult.appendChild(text);

    object_point.appendChild(name);
    object_point.appendChild(pose);
    object_point.appendChild(truncated);
    object_point.appendChild(diffcult);

    QDomElement contour_points=doc.createElement("contour_points");
    for(int i=0;i<image_points_model->rowCount();i++)
    {
        QString p_x=image_points_model->data(image_points_model->index(i,0)).toString();
        QString p_y=image_points_model->data(image_points_model->index(i,1)).toString();


        QDomElement point_i=doc.createElement("point");
        QDomElement x=doc.createElement("x");
        text=doc.createTextNode(p_x);
        x.appendChild(text);
        QDomElement y=doc.createElement("y");
        text=doc.createTextNode(p_y);
        y.appendChild(text);
        QDomElement num=doc.createElement("pointindex");
        text=doc.createTextNode(QString::number(i));
        num.appendChild(text);
        point_i.appendChild(x);
        point_i.appendChild(y);
        point_i.appendChild(num);
        contour_points.appendChild(point_i);

    }
    object_point.appendChild(contour_points);

    root.appendChild(object_point);




    //输出到文件

    QTextStream out_stream(&file);

    doc.save(out_stream,4); //缩进4格

    file.close();


}
void SimpleLableMe::on_pushButton_save_clicked()
{

    QString file_name_image=m_current_filename;
    QStringList file_name_list=file_name_image.split("/");
    QString filename_only=file_name_list[file_name_list.size()-1];
    QStringList filename_only_split=filename_only.split(".");
    write_xml(m_xml_save_dir+"/"+filename_only_split[0]+".xml");

    m_draw_poly_en=false;
    ui->pushButton_Create_Polygon->setEnabled(true);
}
void SimpleLableMe::restore_last_setting()
{
    QSettings settings("SimpleLabelMe", "soft_v1");

    QString fileName=settings.value("images_dir").toString();
    m_image_dir=fileName;

    QDir dir(fileName);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png"<< "*.bmp";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    for(int i=0;i<files.size();i++)
        qDebug()<<files.at(i)<<endl;

    QStringListModel *model = new QStringListModel(files);
    ui->listView->setModel(model);


    m_xml_save_dir=settings.value("save_labelresult_dir").toString();
    int i=0;

}
void SimpleLableMe::find_existing_xml()
{

    QStringList filenamelist=m_current_filename_relative.split(".");
    QString xml_filename=filenamelist[0]+".xml";
    QString xml_filename_global=m_xml_save_dir+"/"+xml_filename;

    QFile mFile( xml_filename_global);
    if(mFile.exists())
    {
        //
        read_xml_and_show_points(xml_filename_global);

    }
    else
    {
        //

        image_points_model->clear();
        init_Point_Tableview();

    }
}

void SimpleLableMe::read_xml_and_show_points(QString filename)
{

    QDomDocument doc;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"successful open"<<endl;
    }

    if (!doc.setContent(&file))
    {
        file.close();
         qDebug()<<"fail open "<<endl;
    }

    file.close();
    QDomElement root = doc.documentElement();//读取根节点
    qDebug()<<root.nodeName();
    QDomNode node = root.firstChild();//读取第一个子节点
    qDebug()<<node.nodeName();
    QString x,y;
    while (!node.isNull())
    {
        if(node.isElement()) //如果节点是元素
        {
            //转换为元素
            QDomElement element=node.toElement();
            if (!element.isNull())// 节点的确是一个元素
            {
                //输出第一个节点，包括第一个节点的属性，这个属性需要指定属性值，才能输出，如果没有输出空
                qDebug()<<element.tagName()<<endl;
                QDomNodeList list=element.childNodes();
                for(int i=0;i<list.count();++i)
                {
                    QDomNode n=list.at(i);
                    //node = list.at(i);
                    if(n.isElement())
                    {
                        QString node_name=n.nodeName();
                        if(node_name.compare("contour_points")==0)
                        {
                            QDomElement element_sub=n.toElement();
                            if (!element_sub.isNull())// 节点的确是一个元素
                            {
                                //输出第一个节点，包括第一个节点的属性，这个属性需要指定属性值，才能输出，如果没有输出空
                                qDebug()<<element_sub.tagName()<<endl;
                                QDomNodeList list_sub=element_sub.childNodes();
                                for(int j=0;j<list_sub.count();++j)
                                {
                                    QDomNode n_sub=list_sub.at(j);
                                    //node = list.at(i);
                                    if(n_sub.isElement())
                                    {
                                        QString node_name=n_sub.nodeName();
//                                        qDebug()<<n_sub.nodeName()<<":"<<n_sub.toElement().text()<<endl;

                                        QDomElement element_sub_sub=n_sub.toElement();
                                        QDomNodeList list_sub_sub=element_sub_sub.childNodes();
                                        for(int k=0;k<list_sub.count();++k)
                                        {
                                            QDomNode n_sub_sub=list_sub_sub.at(k);
                                            if(n_sub_sub.isElement())
                                            {
                                                qDebug()<<n_sub_sub.nodeName()<<":"<<n_sub_sub.toElement().text()<<endl;

                                                QString node_name=n_sub_sub.nodeName();
                                                if(node_name.compare("x")==0)
                                                {
                                                    x=n_sub_sub.toElement().text();

                                                }
                                                if(node_name.compare("y")==0)
                                                {
                                                    y=n_sub_sub.toElement().text();

                                                }
                                                if(node_name.compare("pointindex")==0)
                                                {
                                                    int index=n_sub_sub.toElement().text().toInt();
                                                    image_points_model->setItem(index,0,new QStandardItem(x));
                                                    image_points_model->setItem(index,1,new QStandardItem(y));

                                                    draw_circle(x.toInt(),y.toInt(),5, ui->label_img);

                                                }
                                            }

                                        }

                                    }
                                }
                            }

                        }
                        else
                        {
                            qDebug()<<n.nodeName()<<":"<<n.toElement().text()<<endl;
                            element = n.toElement();
                            //qDebug()<<element.nodeName()<<":"<<element.toElement().text();
                            if (QString::compare(element.tagName(), QStringLiteral("x")) == 0)
                            {
                                // ...处理命令
                                //cout<< "处理命令作者"<<endl;
                            }
                            else if (QString::compare(element.tagName(), QStringLiteral("y")) == 0)
                            {
                                //cout<<"处理命令时间"<<endl;
                                // ...处理命令
                            }
                        }

                    }

                }
            }
        }
        //下一个兄弟节点
        node=node.nextSibling();

    }


    int i=0;


}
void SimpleLableMe::on_pushButton_Next_clicked()
{
    m_index=0;
    m_list_index=m_list_index+1;
    ui->listView->setCurrentIndex(ui->listView->model()->index(m_list_index,0));

    QSettings settings("SimpleLabelMe", "soft_v1");

    QString file_dir=settings.value("images_dir").toString();

    QString str=ui->listView->model()->data(ui->listView->model()->index(m_list_index,0)).toString();
    qDebug()<<str<<endl;

    QString filename=file_dir+"/"+str;
    m_current_filename_relative=str;
    m_current_filename_path=file_dir;
    Load_img_on_Label(ui->label_img,filename,true,ui->widget_img);

    find_existing_xml();
    m_img=cv::imread(filename.toStdString());
}

void SimpleLableMe::on_pushButton_Previous_clicked()
{
    m_index=0;
    m_list_index=m_list_index-1;
    ui->listView->setCurrentIndex(ui->listView->model()->index(m_list_index,0));

    QSettings settings("SimpleLabelMe", "soft_v1");

    QString file_dir=settings.value("images_dir").toString();

    QString str=ui->listView->model()->data(ui->listView->model()->index(m_list_index,0)).toString();
    qDebug()<<str<<endl;

    QString filename=file_dir+"/"+str;
    m_current_filename_relative=str;
    m_current_filename_path=file_dir;
    Load_img_on_Label(ui->label_img,filename,true,ui->widget_img);

    find_existing_xml();
    m_img=cv::imread(filename.toStdString());
}

void SimpleLableMe::on_pushButton_Create_Polygon_clicked()
{
    m_draw_poly_en=true;
    ui->pushButton_Create_Polygon->setEnabled(false);
}

void SimpleLableMe::on_tableView_points_doubleClicked(const QModelIndex &index)
{


    int row=index.row();
    QString x=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row,0)).toString();
    QString y=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row,1)).toString();
    draw_circle(x.toInt(),y.toInt(),25, ui->label_img);
}

void SimpleLableMe::on_pushButton_show_contour_clicked()
{
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Point> c1;
    for(int i=0;i<image_points_model->rowCount();i++)
    {
        QString p_x=image_points_model->data(image_points_model->index(i,0)).toString();
        QString p_y=image_points_model->data(image_points_model->index(i,1)).toString();


        cv::Point point;
        point.x=p_x.toInt();
        point.y=p_y.toInt();
        c1.push_back(point);

    }
    contours.push_back(c1);

    QImage tmp(ui->label_img->pixmap()->toImage());
    cv::Mat img=QImage2Mat(tmp);
//    cv::namedWindow("img 0",CV_WINDOW_FULLSCREEN);
//    cv::Mat img_resize;
//    cv::resize(m_img, img_resize, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
//    cv::imshow("img 0",img_resize);
    cv::drawContours(m_img,contours,0,cv::Scalar(255,0,0),-1);
//    cv::resize(m_img, img_resize, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
//    cv::imshow("img",img_resize);

//    QImage q_img=Mat2QImage(m_img);
    Load_img_on_Label(ui->label_img,m_img,true,ui->widget_img);



}
QImage SimpleLableMe::Mat2QImage(cv::Mat const& src)
{

    cv::Mat temp;
    // make the same cv::Mat
    cvtColor(src, temp,CV_BGR2RGB);
    // cvtColor Makes a copt, that what i need
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();
    // enforce deep copy, see documentation
    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}
cv::Mat SimpleLableMe::QImage2Mat(QImage const& src)
{
    cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
    cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
    cvtColor(tmp, result,CV_BGR2RGB);
    return result;
}

void SimpleLableMe::on_pushButton_MoveUp_clicked()
{
    int row=ui->tableView_points->currentIndex().row();
    if(row>0)
    {
        int row_sub=row-1;
        QString x=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row,0)).toString();
        QString y=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row,1)).toString();
        QString x_up=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row_sub,0)).toString();
        QString y_up=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row_sub,1)).toString();

        image_points_model->setItem(row,0,new QStandardItem(x_up));
        image_points_model->setItem(row,1,new QStandardItem(y_up));
        image_points_model->setItem(row_sub,0,new QStandardItem(x));
        image_points_model->setItem(row_sub,1,new QStandardItem(y));
    }
}

void SimpleLableMe::on_pushButton_Down_clicked()
{
    int row=ui->tableView_points->currentIndex().row();
    if(row<image_points_model->rowCount())
    {
        int row_add=row+1;
        QString x=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row,0)).toString();
        QString y=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row,1)).toString();
        QString x_down=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row_add,0)).toString();
        QString y_down=ui->tableView_points->model()->data(ui->tableView_points->model()->index(row_add,1)).toString();

        image_points_model->setItem(row,0,new QStandardItem(x_down));
        image_points_model->setItem(row,1,new QStandardItem(y_down));
        image_points_model->setItem(row_add,0,new QStandardItem(x));
        image_points_model->setItem(row_add,1,new QStandardItem(y));
    }
}

void SimpleLableMe::on_pushButton_del_clicked()
{
    int row=ui->tableView_points->currentIndex().row();
   image_points_model->removeRow(row);

}

void SimpleLableMe::on_listView_indexesMoved(const QModelIndexList &indexes)
{

}

void SimpleLableMe::on_listView_clicked(const QModelIndex &index)
{

}
