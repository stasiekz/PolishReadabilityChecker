from django.db import models
from django.contrib.postgres.fields import JSONField


class TimeStampedModel(models.Model):
    created = models.DateTimeField(auto_now_add=True)
    updated = models.DateTimeField(auto_now=True)

    class Meta:
        abstract = True


class Analysis(TimeStampedModel):
    file = models.FileField(upload_to="texts/")
    md5_checksum = models.CharField(max_length=32, unique=True)
    stats = JSONField()
    html_text = models.TextField()

    def __str__(self):
        return self.md5_checksum
